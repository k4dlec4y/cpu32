#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"

int nop(struct cpu *cpu);

int halt(struct cpu *cpu);

int add(struct cpu *cpu);

int sub(struct cpu *cpu);

int mul(struct cpu *cpu);

int div0(struct cpu *cpu);

int inc(struct cpu *cpu);

int dec(struct cpu *cpu);

int loop(struct cpu *cpu);

int movr(struct cpu *cpu);

int load(struct cpu *cpu);

int store(struct cpu *cpu);

int in(struct cpu *cpu);

int get(struct cpu *cpu);

int out(struct cpu *cpu);

int put(struct cpu *cpu);

int swap(struct cpu *cpu);

int push(struct cpu *cpu);

int pop(struct cpu *cpu);

int (*instructions[19]) (struct cpu *) = 
{
    &nop, &halt, &add, &sub, &mul,
    &div0, &inc, &dec, &loop, &movr,
    &load, &store, &in, &get, &out,
    &put, &swap, &push, &pop
};

#endif  // INSTRUCTIONS_H
