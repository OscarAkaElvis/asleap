/*
 * asleap - recover weak LEAP passwords.  Pronounced "asleep".
 *
 * Copyright (c) 2004, Joshua Wright <jwright@hasborg.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See COPYING for more
 * details.
 *
 * asleap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Significant code is graciously taken from the following:
 * MS-CHAPv2 and attack tools by Jochen Eisinger, Univ. of Freiburg
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include "common.h"
#include "version.h"
#include "utils.h"

#define PROGNAME "genkeys"

/* Globals */
unsigned long long sortcount;

int compfunc(const void *x, const void *y)
{

    struct hashpass_rec rec1, rec2;

    sortcount++;
    rec1 = (struct hashpass_rec)(*(struct hashpass_rec *)x);
    rec2 = (struct hashpass_rec)(*(struct hashpass_rec *)y);

    if (rec1.hash[15] == rec2.hash[15]) {
        return (0);
    } else if (rec1.hash[15] < rec2.hash[15]) {
        return (-1);
    } else {
        return (1);
    }
}

/* getnextrec accepts a record structure, a file pointer and a flag to indicate
   if we want to populate the password member of the structure (requires a
   malloc).  getnextrec populates the structure with the next record available,
   and returns the record length on success, zero on EOF before a new record,
   or negative on failure. */
int getnextrec(struct hashpass_rec *rec, FILE * fp, int fillpass)
{
    int passlen;

    if (fread(&rec->rec_size, 1, 1, fp) != 1)
        return (feof(fp) ? 0 : -1);

    passlen = (rec->rec_size - 17);
    if (passlen < 0)
        return (-1);

    if (fillpass) {
        if ((rec->password = malloc(passlen + 1)) == NULL) {
            return (-1);
        }
        memset(rec->password, 0, passlen + 1);

        if (fread(rec->password, 1, passlen, fp) != (size_t)passlen) {
            free(rec->password);
            rec->password = NULL;
            return (-1);
        }

    } else {

        /* Skip past the password */
        if (fseek(fp, passlen, SEEK_CUR) != 0)
            return (-1);
    }

    if (fread(rec->hash, 1, sizeof(rec->hash), fp) != sizeof(rec->hash)) {
        if (fillpass) {
            free(rec->password);
            rec->password = NULL;
        }
        return (-1);
    }

    return (rec->rec_size);
}

/* closebuckets accepts an array of type hashbucket_rec and closes each
   file handle.  There is no return value. */
void closebuckets(struct hashbucket_rec hbucket[256], int hsub)
{

    char bucketfile[256];
    for (hsub--; hsub != -1; hsub--) {
        if (hbucket[hsub].sbucket != NULL) {
            fclose(hbucket[hsub].sbucket);
            hbucket[hsub].sbucket = NULL;
        }
        sprintf(bucketfile, "genk-bucket-%02x.tmp", hsub);
        remove(bucketfile);
    }
}

void usage(char *message)
{

    if (strlen(message) > 0) {
        printf("%s: %s\n", PROGNAME, message);
    }

    printf("Usage: %s [options]\n", PROGNAME);
    printf("\n"
           "\t-r \tInput dictionary file, one word per line\n"
           "\t-f \tOutput pass+hash filename\n"
           "\t-n \tOutput index filename\n"
           "\t-h \tLast 2 hash bytes to filter with (optional)\n" "\n");
}

int main(int argc, char *argv[])
{

    char password[MAX_NT_PASSWORD + 1];
    unsigned char pwhash[MD4_SIGNATURE_SIZE];
    int passlen, getnextret, c;
    size_t password_len;
    off_t recoffset;
    float elapsed = 0;
    unsigned long int wordcount = 0;
    struct timeval start, end;
    FILE *inputfl, *outputfl, *outputidx;
    struct hashpass_rec rec, tmprec;
    struct hashpassidx_rec idxrec;
    char wordlist[1025], datafile[1025], indexfile[1025];

    char bucketfile[255];
    unsigned int hsub = 0, brecsub = 0, bucketrecords;
    struct hashbucket_rec hbucket[256];
    struct hashpass_rec *brec;

    printf("genkeys %s - generates lookup file for asleap. "
           "<jwright@hasborg.com>\n", VER);

    while ((c = getopt(argc, argv, "r:f:n:h:")) != EOF) {
        switch (c) {
        case 'r':
            strncpy(wordlist, optarg, sizeof(wordlist) - 1);
            break;
        case 'f':
            strncpy(datafile, optarg, sizeof(datafile) - 1);
            break;
        case 'n':
            strncpy(indexfile, optarg, sizeof(indexfile) - 1);
            break;
        default:
            usage("");
            exit(1);
        }
    }

    if (IsBlank(wordlist) || IsBlank(datafile) || IsBlank(indexfile)) {
        usage("Must supply -r -f and -n");
        exit(1);
    }

    if (*wordlist == '-') {
        printf("Using STDIN for words.\n");
        inputfl = stdin;
    } else {
        if ((inputfl = fopen(wordlist, "rb")) == NULL) {
            perror("fopen");
            exit(1);
        }
    }

    if ((outputfl = fopen(datafile, "wb")) == NULL) {
        perror("fopen");
        fclose(outputfl);
        exit(1);
    }

    if ((outputidx = fopen(indexfile, "wb")) == NULL) {
        perror("fopen");
        fclose(outputfl);
        exit(1);
    }

    /* Create the appropriate buckets for the output files to be used for
       sorting the pass+hash file */

    memset(hbucket, 0, sizeof(hbucket));
    memset(bucketfile, 0, sizeof(bucketfile));

    printf("Generating hashes for passwords (this may take some time) ...");
    fflush(stdout);

    gettimeofday(&start, 0);

    /* create the bucket files */
    for (hsub = 0; hsub < 256; hsub++) {
        sprintf(bucketfile, "genk-bucket-%02x.tmp", hsub);
        if ((hbucket[hsub].sbucket = fopen(bucketfile, "wb")) == NULL) {
            /* Crap, now we have to back-track to close everything we opened */
            closebuckets(hbucket, hsub);
            fprintf(stderr, "Error creating bucket files. ");
            perror("fopen");
            exit(1);
        }
    }

    while (fgets(password, MAX_NT_PASSWORD + 1, inputfl) != NULL) {
        password_len = strlen(password);
        if (password_len > 0 && password[password_len - 1] == '\n')
            password[--password_len] = 0;

        /* Code to accommodate Windows-formatted dictionary files on Linux.
           Thanks ocnarfid8/#kismet.
         */
        if (password_len > 0 && password[password_len - 1] == '\r')
            password[--password_len] = 0;

#ifndef _OPENSSL_MD4
        /* md4.c seems to have a problem with passwords longer than 31 bytes.
           This seems odd to me, but it should have little impact on our
           final product, since I assume there are few passwords we will be
           able to identify with a dictionary attack that are longer than 31
           bytes. */
        password[31] = 0;
#endif

        password_len = strlen(password);
        NtPasswordHash(password, password_len, pwhash);

        memcpy(rec.hash, pwhash, sizeof(rec.hash));
        rec.rec_size = (password_len + sizeof(rec.hash)
                + sizeof(rec.rec_size));

        /* Write the output record to the correct bucket, depending on byte
           14 of the hash value. */
        hsub = rec.hash[14];
        fwrite(&rec.rec_size, sizeof(rec.rec_size), 1,
               hbucket[hsub].sbucket);
        fwrite(password, password_len, 1, hbucket[hsub].sbucket);
        fwrite(&rec.hash, sizeof(rec.hash), 1, hbucket[hsub].sbucket);

        hbucket[hsub].numrec++;
        wordcount++;
    }

    if (ferror(inputfl)) {
        perror("fgets");
        fclose(inputfl);
        closebuckets(hbucket, 256);
        fclose(outputfl);
        fclose(outputidx);
        exit(-1);
    }

    /* Flush all buffers before closing -- I don't think this is really
       necessary, but it's kind of a belt-and-suspenders thing.  No one ever
       said I was a snappy dresser. */
    fflush(NULL);
    fclose(inputfl);

    /* Close and reopen all the buckets (they were in "w" mode) */

    for (hsub = 0; hsub < 256; hsub++) {
        sprintf(bucketfile, "genk-bucket-%02x.tmp", hsub);
        fclose(hbucket[hsub].sbucket);
        if ((hbucket[hsub].sbucket = fopen(bucketfile, "rb")) == NULL) {
            closebuckets(hbucket, hsub);
            fprintf(stderr, "Error re-opening bucket files.");
            perror("fopen");
            exit(1);
        }
    }

    printf("Done.\n");

    /* Report stats on writing hashes */
    gettimeofday(&end, 0);

    /* Borrowed from Tim Newsham's wep_crack.c -- thanks Tim */
    if (end.tv_usec < start.tv_usec) {
        end.tv_sec -= 1;
        end.tv_usec += 1000000;
    }
    end.tv_sec -= start.tv_sec;
    end.tv_usec -= start.tv_usec;
    elapsed = end.tv_sec + end.tv_usec / 1000000.0;

    printf("%lu hashes written in %.2f seconds:  %.2f hashes/second\n",
           wordcount, elapsed, wordcount / elapsed);

    printf("Starting sort (be patient) ...");
    fflush(stdout);

    hsub = 0;
    while (hsub < 256) {
        sprintf(bucketfile, "genk-bucket-%02x.tmp", hsub);

        /* If there are no records in the bucket, unlink */
        if (hbucket[hsub].numrec == 0) {
            /* Bucket is empty, get rid if it */
            fclose(hbucket[hsub].sbucket);
            hbucket[hsub].sbucket = NULL;
            unlink(bucketfile);
            hsub++;
            continue;
        }

/*
 * With all our buckets created, clean up and then sort each bucket.  After
 * Sorting the bucket, write the output file to the outputfl handle for one
 * big file.
 */

        /* malloc() enough space in memory for each record that we have to
           load into memory for sorting */
        if ((brec = calloc(hbucket[hsub].numrec, sizeof(rec))) == NULL) {
            fprintf(stderr,
                "Unable to allocate %ld bytes of memory.  ",
                (hbucket[hsub].numrec * sizeof(rec)));
            closebuckets(hbucket, 256);
            perror("malloc");
            exit(-1);
        }

        bucketrecords = hbucket[hsub].numrec;
        for (brecsub = 0; brecsub < bucketrecords; brecsub++) {

            /* Get 1 byte for the record length */
            if (fread(&brec[brecsub].rec_size, 1, 1,
                  hbucket[hsub].sbucket) != 1) {
                fprintf(stderr,
                    "Unable to read from bucket %02x. ",
                    hsub);
                perror("fread");
                closebuckets(hbucket, 256);
                exit(-1);
            }

            /* Calculate password length - 17 is hash+rec length byte */
            passlen = (brec[brecsub].rec_size - 17);
            if (passlen < 0) {
                fprintf(stderr,
                    "Invalid record length in bucket %02x.\n", hsub);
                closebuckets(hbucket, 256);
                exit(-1);
            }

            /* malloc() memory for this password based on password length */
            if ((brec[brecsub].password =
                 malloc(passlen + 1)) == NULL) {
                fprintf(stderr,
                    "Cannot allocate %d bytes of memory. ",
                    (passlen + 1));
                perror("malloc");
                closebuckets(hbucket, 256);
                exit(-1);
            }
            memset(brec[brecsub].password, 0, passlen + 1);

            /* Populate the password field with the next parameter in the
               record */
            if (fread(brec[brecsub].password, 1, passlen,
                      hbucket[hsub].sbucket) != (size_t)passlen) {
                fprintf(stderr,
                    "Incomplete password record in bucket %02x.\n", hsub);
                closebuckets(hbucket, 256);
                exit(-1);
            }

            /* Populate the hash field with the final parameter in the record */
            if (fread(brec[brecsub].hash, 1, sizeof(brec[brecsub].hash),
                      hbucket[hsub].sbucket) !=
                sizeof(brec[brecsub].hash)) {
                fprintf(stderr,
                    "Incomplete hash record in bucket %02x.\n", hsub);
                closebuckets(hbucket, 256);
                exit(-1);
            }
        }

        /* sort this bucket */
        qsort(brec, hbucket[hsub].numrec, sizeof(rec), compfunc);

        /* Write the sorted records to the outputfl handle */
        for (brecsub = 0; brecsub < bucketrecords; brecsub++) {
            passlen = brec[brecsub].rec_size - 17;
            fwrite(&brec[brecsub].rec_size, sizeof(rec.rec_size), 1,
                   outputfl);
            fwrite(brec[brecsub].password, passlen, 1, outputfl);
            fwrite(brec[brecsub].hash, sizeof(rec.hash), 1,
                   outputfl);
            free(brec[brecsub].password);
        }

        /* Get rid of the bucket */
        fclose(hbucket[hsub].sbucket);
        hbucket[hsub].sbucket = NULL;
        sprintf(bucketfile, "genk-bucket-%02x.tmp", hsub);
        remove(bucketfile);
        free(brec);

        hsub++;

    }

    fflush(outputfl);
    fclose(outputfl);

    printf("Done.\nCompleted sort in %llu compares.\n", sortcount);
    printf("Creating index file (almost finished) ...");
    fflush(stdout);

    /* Re-open output file "r" */
    if ((outputfl = fopen(datafile, "rb")) == NULL) {
        perror("fopen");
        fclose(outputfl);
        exit(1);
    }

/* Create the index file - populate an initial record, then keep reading
 * through the file.  When the last 2 bytes of the hash change, write the
 * index record to the index file and keep reading records until EOF.
 */

    recoffset = 0;
    getnextret = 0;

    /* Populate the initial index record from the outputfl */
    getnextret = getnextrec(&tmprec, outputfl, 0);
    if (getnextret <= 0) {
        fprintf(stderr, "Unable to read initial password record.\n");
        fclose(outputfl);
        fclose(outputidx);
        exit(-1);
    }

    memset(&idxrec, 0, sizeof(idxrec));
    memset(&rec, 0, sizeof(rec));

    /* Update the offset of the record, this is always 0 for the first record */
    recoffset = recoffset + getnextret;

    idxrec.hashkey[0] = tmprec.hash[14];
    idxrec.hashkey[1] = tmprec.hash[15];
    idxrec.offset = 0;
    idxrec.numrec = 1;

    while ((getnextret = getnextrec(&rec, outputfl, 0)) > 0) {
        recoffset = recoffset + getnextret;

        if (idxrec.hashkey[0] != rec.hash[14] ||
            idxrec.hashkey[1] != rec.hash[15]) {

            /* Finished reading records for this hash key value.  Write the
               index record and populate the next index record. */
            if (fwrite(&idxrec, sizeof(idxrec), 1, outputidx) != 1) {
                perror("Error writing index record");
                fclose(outputfl);
                fclose(outputidx);
            }
            //           fflush(outputidx);
            /*
               printf("DEBUG index record:\n");
               printf("idxrec.hashkey = %02x%02x\n", idxrec.hashkey[0], idxrec.hashkey[1]);
               printf("idxrec.offset = %llu\n", idxrec.offset);
               printf("idxrec.numrec = %llu\n", idxrec.numrec);
               printf("========================================================\n");
             */

            /* Initialize struct for the next index entry set */
            memset(&idxrec, 0, sizeof(idxrec));

            /* Populate the next hash reference and offset */
            idxrec.hashkey[0] = rec.hash[14];
            idxrec.hashkey[1] = rec.hash[15];
            idxrec.offset = recoffset - getnextret;
            idxrec.numrec = 1;
            continue;

        } else {
            /* Got another record with the same hash value, increment count */
            idxrec.numrec++;
            continue;
        }
    }

    if (getnextret < 0) {
        fprintf(stderr, "Incomplete password record while creating index.\n");
        fclose(outputfl);
        fclose(outputidx);
        exit(-1);
    }

    /* Write the final index record */
    if (fwrite(&idxrec, sizeof(idxrec), 1, outputidx) != 1) {
        perror("Error writing final index record");
        fclose(outputfl);
        fclose(outputidx);
        exit(-1);
    }

    fclose(outputfl);
    fclose(outputidx);

    printf("Done.\n");
    fflush(stdout);

    exit(0);

}
