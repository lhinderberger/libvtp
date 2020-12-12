# Release Log of libvtp


## v0.3.0 - In development
### Summary
TODO

### Additions
- The functions vtp_read_instruction_words and vtp_write_instruction_words were
  added to provide a single source of truth for reading/writing VTP Binary
  to/from byte arrays/streams.

### Modifications
- The specification previously falsely claimed the endianness to be little
  endian, when in fact all VTP software so far has been big endian.
  Changed specified endianness to big endian.
- The vtp-assemble and vtp-disassemble tools and the encode_potis example were
  adapted to use vtp_read_instruction_words / vtp_write_instruction_words
- Removed an unnecessary include of stdio.h in fold.c
- Extended test for the fold algorithm
- README: Added section about versioning and compatibility

## v0.2.0 - 2020-12-08
### Summary
This release provides an Arduino port of libvtp, as well as a bug fix for
embedded / legacy platforms.

### Additions
- A script to prepare an Arduino library zip of libvtp.
- Examples that showcase the use of libvtp on the Arduino platform.

### Modifications
- Fixed a bug that would break instruction encoding on embedded platforms
- .gitignore: Ignoring the `arduino/` build directory.
- NOTICE: Clarification that some distributions may not contain the dependencies
- README.md: Documentation of the Arduino build script

## v0.1.0 - 2020-11-25
### Summary
This initial release of libvtp, based on the current state of the VTPv1 draft 
specification, serves as a base for further experimentation and discussion.