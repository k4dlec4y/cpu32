# cpu32

## Table of Contents
- [Introduction](#introduction)
- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Tests](#tests)
- [License](#license)
- [Author](#author)

## Introduction
This is a refactored version of one of my first university projects in C,
which I originally wrote a year ago. The goal of this refactoring is to apply
what I’ve learned over the past year and to get better at reading and
understanding "unfamiliar" code (one year is a long time). We were given
a `main.c` file to start with, but in this refactored version, I’ll be writing
my own main so that the project feels fully like my own work.

## Overview
This program is an emulator for 32-bit processor with 19 instructions
like add, sub, movr, stack operations and more.

Currently, there is no compiler available for cpu32 programs. To write your 
own, write instructions in hexadecimal digits and then convert the .hex file
into .bin with `xxd -r -p < program.hex > program.bin`.

### Cpu Memory
The memory is allocated in 4 KiB blocks. Instructions are stored at the start
of the memory, and the stack is placed after them. The stack grows downward
when pushing (stack bottom is the highest address). The address space
between the end of instructions and start (top) of the stack is filled
with zeros.

### Instructions
Instructions are represented by 32-bit little-endian numbers. This also
applies to operands like REG, INDEX and NUM. REG A/B/C/D is represented
by 0/1/2/3.

`0 - nop`  
Does nothing (only increases program counter).  
`1 - halt`  
Stops execution and sets CPU status to CPU_HALTED.  
`2 - add REG`  
Adds the value of REG to register A.  
`3 - sub REG`  
Subtracts the value of REG from register A.  
`4 - mul REG`  
Multiplies register A by the value of REG.  
`5 - div REG`  
Divides register A by REG.  
If REG is 0, instruction won’t execute and CPU status is set to CPU_DIV_BY_ZERO.  
`6 - inc REG`  
Increments the value of REG by 1.  
`7 - dec REG`  
Decrements the value of REG by 1.  
`8 - loop INDEX`  
If register C is non-zero, program counter jumps start of the memory + INDEX.  
`9 - movr REG NUM`  
Moves NUM into register REG.  
`10 - load REG NUM`  
Loads into REG the number at STACK_TOP + register D + NUM.  
If the address is beyond the stack, CPU status is set to CPU_INVALID_STACK_OPERATION.  
`11 - store REG NUM`  
Stores REG into STACK_TOP + register D + NUM.  
If the address is beyond the stack, CPU status is set to CPU_INVALID_STACK_OPERATION.  
`12 - in REG`  
Reads a 32-bit number from input into REG.  
If the value doesn't represent a valid number, cpu status is
set to CPU_IO_ERROR.  
If there are no more numbers on the input (EOF), register C is set to 0 and
the value of REG is set to -1 (even if REG is C).  
`13 - get REG`  
Reads a single byte from input into REG.  
If EOF, register C = 0 and REG = -1.  
`14 - out REG`  
Prints the value of REG as a decimal number.  
`15 - put REG`  
Prints REG as a single character if 0 ≤ REG ≤ 255.  
Otherwise, CPU status is set to CPU_ILLEGAL_OPERAND.  
`16 - swap REG REG`  
Swaps the values of two registers.  
`17 - push REG`  
Pushes REG on the stack.  
If the stack is full, CPU status is set to CPU_INVALID_STACK_OPERATION.  
`18 - pop REG`  
Pops a number from the stack into REG.  
If the stack is empty, CPU status is set to CPU_INVALID_STACK_OPERATION.  

### Examples
You can find programs written in assembly in `./data/txt/*.txt`, written
in hexadecimal digits in `./data/bin/*.hex` and binaries in `./data/bin/*.bin`.

## Requirements
- GCC with C99 support
- make
- xxd (for writing cpu32 programs)
- Tested on Linux (Fedora 42)

## Installation
```bash
git clone https://github.com/k4dlec4y/cpu32
cd cpu32
make
```

## Usage
```bash
./build/cpu32 (run|trace) [stack_capacity] FILE
```
where  
- `run` will run the emulator in normal mode and `trace` will print informations
about cpu after every instruction  
- `stack_capacity` is an optional parameter (default is 4096)  
- `FILE` is a path to the file containing the program (binary with instructions)

## Tests
To run simple cli test, execute:  
```bash
./cli_test.sh
```

## License

This project is licensed under the MIT License – see the LICENSE file for details.

## Author

Created by Matúš Kadlecay - https://github.com/k4dlec4y
