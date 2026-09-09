# Changelog

This file records the release and update history of asleap.

## 2.5

 - Replaced the deprecated OpenSSL `MD4()` one-shot function with the EVP digest API, improving compatibility with OpenSSL 3.x.
 - Added various small internal maintenance, documentation, and repository organization improvements.
 - Updated the Makefile to respect externally supplied compiler, preprocessor, and linker flags while preserving the definitions required by asleap.
 - Added validation for dictionary and password database reads to handle EOF, truncated records, and lines without a trailing newline safely.

## 2.4.1

 - Improved OpenSSL 3.x compatibility.
 - Explicitly load the OpenSSL legacy provider required for DES operations.
 - Fix DES encryption initialization failures on systems where the legacy provider is not enabled by default.

## 2.4

 - Fork maintenance release.
 - Added `make install` and `make uninstall` support.
 - Merged the fix from upstream PR #9 to resolve issue #8: Replaced deprecated DES `crypt()` and `setkey()` usage with the OpenSSL EVP API.

## 2020-11-28

 - Due to GLIBC removal of `setkey()` and `crypt()`, link to libxcrypt to restore functionality. Minor code cleanup.

## 2008-05-28

 - Minor update to fix a problem with `IFNAMSIZ` errors when building on some platforms.

## 2007-07-13

 - Added the `-C` and `-W` options, allowing the challenge and response values to be specified as colon-delimited bytes on the command line. This makes asleap a generic MS-CHAPv2 dictionary attack tool.

## 2007-05-10

The original author reported substantial code improvements and removed:

- Removed support for Airopeek NX files. Wireshark can be used to convert `.apc` files to libpcap format.
- Removed Airjack integration for deauthentication.
- Discontinued Windows support.

- Fixed a segmentation fault in `genkeys` with passwords longer than 64 characters.
- Added support for QoS data frames.
- Added support for radiotap-formatted capture files.
