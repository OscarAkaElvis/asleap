LDLIBS		= -lpcap
override CPPFLAGS += -D_LINUX -D_OPENSSL_MD4
CFLAGS		?= -pipe -Wall
LDFLAGS		?=
LDLIBS		+= -lcrypto
PROGOBJ		= asleap.o genkeys.o utils.o common.o sha1.o
PROG		= asleap genkeys
PREFIX		?= /usr
BINDIR		?= $(PREFIX)/bin
DESTDIR		?=

all: $(PROG) $(PROGOBJ)

utils: utils.c utils.h
	$(CC) $(CPPFLAGS) $(CFLAGS) utils.c -c

common: common.c common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) common.c -c

sha1: sha1.c sha1.h
	$(CC) $(CPPFLAGS) $(CFLAGS) sha1.c -c

asleap: asleap.c asleap.h sha1.o common.o common.h utils.o version.h sha1.c sha1.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) asleap.c -o asleap common.o utils.o sha1.o $(LDLIBS)

genkeys: genkeys.c md4.c md4.h common.o utils.o version.h common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) md4.c genkeys.c -o genkeys common.o utils.o $(LDLIBS)

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
