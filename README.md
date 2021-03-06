**This library has been deprecated in favor of [vtproto](https://github.com/lhinderberger/vtproto)**

# libvtp
A library for encoding and decoding vibrotactile patterns

## Summary
libvtp is a library for encoding and decoding vibrotactile patterns using
the (in-development) VTP format.

Specification drafts can be found in the `doc/` subdirectory.

The library contains the following modules:

- **`codec`**
  provides an abstraction layer for reading/writing VTP Binary files
- **`fold`**
  provides a fold algorithm to accumulate the effects of multiple
  VTP instructions, e.g. for the purpose of simulation or mapping VTP to
  a sampling-like interface

Additionally, libvtp contains the CLI tools:

- `vtp-assemble` which assembles VTP Assembly Code into the VTP Binary Format
- `vtp-disassemble` which derives VTP Assembly Code from VTP Binary Format

They are mostly untested and to be considered as strictly experimental at this
point.

## Usage
libvtp should compile on any compiler that conforms to the C90 standard.
If it doesn't in your case, please file a bug report on libvtp's issue tracker
(see section Contact).

libvtp uses CMake as its build system, but it should be trivially possible to
port it to other build systems, if needed.

The library API documentation can be generated using Doxygen, but it's probably
quicker to just read through the very few and brief header files in the
`include/vtp` directory.

Usage information for the CLI tools can be printed using the `--help` option.
Please be aware that they read from stdin by default, so if you run them without
any input, they might appear to be frozen when in fact they're just waiting for
the standard input.

## Arduino Port
It is possible to generate an Arduino library of libvtp using the
`build-arduino-library.sh` script in the root of the source tree.

Running it will build an Arduino .zip library to the `arduino/` subdirectory
which can then be included in the Arduino IDE.

The Arduino Port is automatically generated by the `build-arduino-library.sh`
script and modifies libvtp to flatten the directory structure as expected by
the Arduino IDE.

To include libvtp in your Arduino projects, use `#include <vtp_foo.h>` instead
of `include <vtp/foo.h>`.

## Dependencies
libvtp's unit tests depend on `greatest` by Scott Vokes.

For details about the dependencies used, please refer to
the [NOTICE](./NOTICE) file

## Versioning and Compatibility
Versioning of libvtp follows the [Semantic Versioning](https://semver.org)
convention.

libvtp keeps a changelog in [RELEASES.md](./RELEASES.md)

Until 1.0.0, any substantial changes to the library (breaking and non-breaking)
will trigger an increase of the minor version, while bug fixes and similar minor
improvements may trigger an increase of the patch version.

Additionally, libvtp will strive to achieve ABI compatibility within major
versions, once the stable release v1.0.0 has been published.

## Copyright
libvtp is (C) 2020 Lucas Hinderberger

It is licensed under the Apache Licence Version 2.0.
For details, please refer to the [LICENCE](./LICENSE) file and
the [NOTICE](./NOTICE) file.

## Contact
The repository of libvtp can be found at https://github.com/lhinderberger/libvtp

You're welcome to file bug reports, other issues and pull requests there.

You can also contact the author via email at mail@lhinderberger.com

