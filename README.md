# cpu32

## Table of Contents
- [Introduction](#introduction)
- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
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
This program is an emulator for 32-bit processor with 19 basic arithmetic
instructions like add, sub, movr, push, pop and more (you can find the list
of instructions inside `include/instructions.h`).

Currently, there is no compiler available for cpu32 programs. To write your 
own, use a binary editor such as xxd or hexedit.

## Requirements
- GCC with C99 support
- make
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
about after every instruction  
- `stack_capacity` is an optional parameter (default is 4096)  
- `FILE` is a path to the file containing the program (binary with instructions)

## License

This project is licensed under the MIT License – see the LICENSE file for details.

## Author

Created by Matúš Kadlecay - https://github.com/k4dlec4y
