# Release Log of libvtp

## In Development: v0.1.1
### Summary
The purpose of this release will be to provide an Arduino port of libvtp.

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