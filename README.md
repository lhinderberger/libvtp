# libvtp
A library for encoding and decoding vibrotactile patterns

## Summary
libvtp is a library for encoding and decoding vibrotactile patterns using
the (yet-to-be-specified) VTP format.

Specification drafts can be found in the `doc/` subdirectory.

The library will contain the following modules:

- `codec` provides an abstraction layer for reading/writing VTP Binary files
- `assembler` provides a parser that can read VTP Assembly Code
- `serialize-asm` provides serialization of patterns to VTP Assembly Code

Additionally, libvtp will contain the CLI tools:

- `vtp assemble` which assembles VTP Assembly Code into the VTP Binary Format
- `vtp disassemble` which derives VTP Assembly Code from VTP Binary Format

## Usage
libvtp will use CMake as its build system.

TODO: Further instructions

## Dependencies
libvtp depends on a C90-compliant C-compiler.

For running the unit tests, libvtp also depends on `greatest` by Scott Vokes,
which is supplied in the `vendor/` directory.

## Copyright
libvtp is (C) 2020 Lucas Hinderberger

Licensing will be determined at a later point in time.