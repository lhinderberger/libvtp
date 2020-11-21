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

Additionally, libvtp will contain the CLI tools:

- `vtp assemble` which assembles VTP Assembly Code into the VTP Binary Format
- `vtp disassemble` which derives VTP Assembly Code from VTP Binary Format

## Usage
libvtp should compile on any compiler that conforms to the C90 standard.
If it doesn't in your case, please file a bug report on libvtp's issue tracker
(see section Contact).

libvtp uses CMake as its build system, but it should be trivially possible to
port it to other build systems, if needed.

TODO: Provide library documentation
TODO: Provide CLI usage information

## Dependencies
libvtp's unit tests depend on `greatest` by Scott Vokes.

For details about the dependencies used, please refer to
the [NOTICE](./NOTICE) file

## Copyright
libvtp is (C) 2020 Lucas Hinderberger

It is licensed under the Apache Licence Version 2.0.
For details, please refer to the [LICENCE](./LICENSE) file and
the [NOTICE](./NOTICE) file.

## Contact
The repository of libvtp can be found at https://github.com/lhinderberger/libvtp

You're welcome to file bug reports, other issues and pull requests there.

You can also contact the author via email at mail@lhinderberger.com

