# Compiling the Source

To compile this source on Debian-like systems, install the base build tools,
libpcap headers, and OpenSSL development headers:

```
$ sudo apt update && sudo apt install build-essential libpcap-dev libssl-dev -y
```

To build this tool, change to the `asleap` directory and run `make`:

```
$ make
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3    -c -o sha1.o sha1.c
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3    -c -o common.o common.c
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3    -c -o utils.o utils.c
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3     asleap.c -o asleap common.o utils.o sha1.o -lpcap -lcrypto
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3     md4.c genkeys.c -o genkeys common.o utils.o -lpcap -lcrypto
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3    -c -o asleap.o asleap.c
gcc -pipe -Wall -D_LINUX -D_OPENSSL_MD4 -g3    -c -o genkeys.o genkeys.c
```

Optionally you can edit the `Makefile` and change the compiler to clang as desired.

## Installing

By default, `make install` installs `asleap` and `genkeys` into `/usr/bin`:

```
$ sudo make install
```

To uninstall:

```
$ sudo make uninstall
```

If needed, you can override the install root/prefix:

```
$ make DESTDIR=/tmp/pkgroot install
$ make PREFIX=/usr/local install
```
