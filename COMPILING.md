# Compiling the Source

To compile this source on Debian-like systems, install the base build tools, libpcap headers, and OpenSSL development headers:

```sh
sudo apt update
sudo apt install -y build-essential libpcap-dev libssl-dev
```

To build asleap, change to the source directory and run:

```sh
make clean
make
```

The Makefile uses the standard `CC`, `CPPFLAGS`, `CFLAGS`, and `LDFLAGS` variables. For example, to build with Clang:

```sh
make clean
make CC=clang
```

Custom build flags can also be supplied without editing the Makefile:

```sh
make clean
make CFLAGS="-O2 -Wall -Wextra" LDFLAGS="-Wl,-z,relro -Wl,-z,now"
```

## Installing

By default, `make install` installs `asleap` and `genkeys` into `/usr/bin`:

```sh
sudo make install
```

To uninstall:

```sh
sudo make uninstall
```

If needed, you can override the install root/prefix:

```sh
make DESTDIR=/tmp/pkgroot install
sudo make PREFIX=/usr/local install
```
