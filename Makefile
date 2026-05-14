##################################
# <jwright> Well, I may be doing stupid things with make
# <jwright> OK, it was Makefile stupid'ness
# <jwright> I don't really understand what the hell I am doing with Make, I'm
#           just copying other files and seeing what works.
# <dragorn> heh
# <dragorn> i think thats all anyone does
# <dragorn> make is a twisted beast
##################################
LDLIBS		= -lpcap
CFLAGS		= -pipe -Wall -D_LINUX -D_OPENSSL_MD4
LDLIBS		+= -lcrypto
CFLAGS		+= -g3 #-ggdb -g
PROGOBJ		= asleap.o genkeys.o utils.o common.o sha1.o
PROG		= asleap genkeys
#CC		    = clang-10
CC          = gcc
PREFIX		?= /usr
BINDIR		?= $(PREFIX)/bin
DESTDIR		?=

all: $(PROG) $(PROGOBJ)

utils: utils.c utils.h
	$(CC) $(CFLAGS) utils.c -c 

common: common.c common.h
	$(CC) $(CFLAGS) common.c -c

sha1: sha1.c sha1.h
	$(CC) $(CFLAGS) sha1.c -c

asleap: asleap.c asleap.h sha1.o common.o common.h utils.o version.h sha1.c \
	sha1.h 
	$(CC) $(CFLAGS) asleap.c -o asleap common.o utils.o sha1.o $(LDLIBS)

genkeys: genkeys.c md4.c md4.h common.o utils.o version.h common.h
	$(CC) $(CFLAGS) md4.c genkeys.c -o genkeys common.o utils.o $(LDLIBS)

install: $(PROG)
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(PROG) $(DESTDIR)$(BINDIR)

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/asleap $(DESTDIR)$(BINDIR)/genkeys

clean:
	$(RM) $(PROGOBJ) $(PROG) *~

strip:
	@ls -l $(PROG)
	@strip $(PROG)
	@ls -l $(PROG)
