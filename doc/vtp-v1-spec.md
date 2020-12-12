+++ DRAFT SPECIFICATION +++

# VTP v1 Specification

## Summary
The VTP specification specifies representations of a data format for storing
and conveying patterns for vibrotactile displays.

Due to its design, VTP can also be viewed as a domain-specific machine language
for rendering vibrotactile patterns - a property that is used to provide a
human-readable assembly representation.

VTP is an acronym for VibroTactile Pattern.


In its current form, VTP is capable of representing vibrotactile patterns with

- up to 255 actuators (channels)
- a time resolution of 1ms
- a frequency range of 0-1023 Hz with a resolution of 1Hz
- an amplitude resolution of 1024 steps


## Binary Representation
### Overview
The VTP Binary Format for vibrotactile patterns is designed to fulfill
the following goals:

- Efficiently represent vibrotactile patterns for displays with
  a low to medium number of actuators
- Explicitly represent both amplitude and frequency for each actuator
- Allow streaming of vibrotactile patterns
- Be lightweight enough for efficient decoding on microcontrollers

It is intended to be wrapped within a container format that for example can
provide additional metadata or synchronization with other media streams.

Compression and Modulation capabilities are not decided yet.

### Structure
Due to the similarity of VTP Binary Format with machine language, this section
will use familiar terminology from the field of Computer Architecture that is
used there to describe instruction set architectures, as introduced for example
in [PattersonHennessy2018](#PattersonHennessy2018).

A VTP Binary file is a stream of VTP Instruction Words, with each instruction
word having a size of 32 bits.

When writing to / reading from byte streams, VTP Instruction Words are to be
treated as big endian.

Each instruction word begins with a 4 bit instruction code, which in turn
determines the instruction format used.

Code  | Instruction    | Instr. Format
------|----------------|---------------
0000  | Increment time | Format A
0001  | Set frequency  | Format B
0010  | Set amplitude  | Format B
other | Reserved       | -

#### Instruction Formats
##### Instruction Format A
```
Content   | Instr.  | Parameter A
          | code    |
Bit       32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17

Content   Parameter A (contd.)                          |
Bit       16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1
```

- 4bit Instruction Code
- 28bit Parameter A

##### Instruction Format B
```
Content   | Instr.  | Channel Select        | Time
          | code    |                       | Offset
Bit       32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17

Content   Time Offset     | Parameter A                 |
          (contd.)        |                             |
Bit       16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1
```

- 4bit Instruction Code
- 8bit Channel Select (unsigned integer)
- 10bit Time Offset (unsigned integer)
- 10bit Parameter A

### Instructions
#### 0000 - Increment time
This instruction increments the time counter by the offset given in parameter A,
interpreted as an unsigned integer, in milliseconds.

Initially, the time counter has the value zero.

#### 0001 - Set frequency
This instruction sets the frequency of one or all channels, based on:

- Channel Select - The number of the channel (actuator), or all channels if zero
- Parameter A    - The new frequency, in Hertz

If Time Offset is non-zero, the time counter will be increased by that amount of
milliseconds.

#### 0010 - Set amplitude
This instruction sets the amplitude of one or all channels, based on:

- Channel Select - The number of the channel, or all channels if zero.
- Parameter A    - The new amplitude, in 1/1024ths of the maximum amplitude
                   in an amplitude range of 0 to 1023/1024

If Time Offset is non-zero, the time counter will be increased by that amount of
milliseconds.


## Assembly Representation
A more human-friendly representation of VTP Binary instructions comes in the
form of VTP Assembly Code.

A VTP pattern can be transformed back and forth between the assembly code
and its binary representation (with only comments and formatting being lost).

For example, the assembly code

```
freq ch* 234
amp ch* 123
freq ch2 345

freq +50ms ch2 456
freq ch1 789

time +2000ms
amp ch* 234
freq ch2 567
```

maps to the binary code

```
0x100000ea
0x2000007b
0x10200159
0x1020c9c8
0x10100315
0x000007d0
0x200000ea
0x10200237
```

with each instruction word represented as a C-style hexadecimal number.

### ABNF Grammar
```abnf
pattern = *instruction
instruction = format-a / format-b / comment / empty-line

format-a = "time" 1*WSP time-offset line-end
format-b = ("freq" / "amp") 1*WSP [time-offset] 1*WSP channel-select 1*WSP number line-end

channel-select = "ch" ("*" / number)
time-offset = "+" number "ms"

comment = "--" *VCHAR LF
empty-line = 1*WSP LF
number = *DIGIT
line-end = 1*WSP (comment / LF)
```

With terminal rules as defined in Appendix B of [RFC5234](#RFC5234).

Note that this grammar does not contain any range checking for numbers.
You should perform a validation step after parsing.


## Literature
<a name="PattersonHennessy2018"></a>[PattersonHennessy2018] Patterson David A., Hennessy, John L. "Computer Organization and Design: The Hardware/Software Interface RISC-V Edition", Cambridge, MA: Elsevier, Morgan Kaufmann Publishers, 2018

<a name="RFC5234"></a>[RFC5234] Crocker, D., Overell P., "Augmented BNF for Syntax Specifications: ABNF", RFC 5234, RFC Editor, http://www.rfc-editor.org/info/rfc5234, January 2008