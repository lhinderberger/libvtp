# Release Log of libvtp


## v???? - In development
### Summary
TODO

### Modifications
- The specification previously falsely claimed the endianness to be little
  endian, when in fact all VTP software so far has been big endian.
  Changed specified endianness to big endian.
- Removed an unnecessary include of stdio.h in fold.c
- Extended test for the fold algorithm
- README: Added section about versioning and compatiblity

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