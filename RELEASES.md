# Release Log of libvtp


## v???? - In development
### Summary
TODO

### Modifications
- Removed an unnecessary include of stdio.h in fold.c

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