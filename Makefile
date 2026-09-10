LDLIBS		= -lpcap
override CPPFLAGS += -D_LINUX -D_OPENSSL_MD4
CFLAGS		?= -pipe -Wall
LDFLAGS		?=
LDLIBS		+= -lcrypto
PROGOBJ		= asleap.o genkeys.o utils.o common.o sha1.o
PROG		= asleap genkeys
DOCS		= README.md THANKS.md COMPILING.md CONTRIBUTING.md LICENSE
PREFIX		?= /usr
BINDIR		?= $(PREFIX)/bin
DOCDIR		?=
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

genkeys: genkeys.c common.o utils.o version.h common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) genkeys.c -o genkeys common.o utils.o $(LDLIBS)

install: $(PROG)
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(PROG) $(DESTDIR)$(BINDIR)
ifdef DOCDIR
	install -d $(DESTDIR)$(DOCDIR)
	install -m 0644 $(DOCS) $(DESTDIR)$(DOCDIR)
endif

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/asleap $(DESTDIR)$(BINDIR)/genkeys
ifdef DOCDIR
	$(RM) $(addprefix $(DESTDIR)$(DOCDIR)/,$(DOCS))
	-rmdir $(DESTDIR)$(DOCDIR)
endif

clean:
	$(RM) $(PROGOBJ) $(PROG) *~

strip:
	@ls -l $(PROG)
	@strip $(PROG)
	@ls -l $(PROG)
