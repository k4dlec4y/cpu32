#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdio.h>

enum cpu_status {
    CPU_OK,
    CPU_HALTED,
    CPU_ILLEGAL_INSTRUCTION,
    CPU_ILLEGAL_OPERAND,
    CPU_INVALID_ADDRESS,
    CPU_INVALID_STACK_OPERATION,
    CPU_DIV_BY_ZERO,
    CPU_IO_ERROR
};

enum cpu_register {
    REGISTER_A,
    REGISTER_B,
    REGISTER_C,
    REGISTER_D,
};

struct cpu {
    int32_t arithmetic_regs[4];
    enum cpu_status status;
    int32_t instruction_index;
    size_t stack_size;

    int32_t* mem_start_address;
    int32_t* stack_top;
    int32_t* stack_bottom;
    int8_t has_stack;

    // stack roof is the lowest valid stack adress (closest to instructions)
    int32_t* stack_roof;

    // this is here for in instruction, if there is negative number
    // after another number without whitespace, i need to save it for next in
    int8_t is_neg;
    
};

int32_t* cpu_create_memory(FILE *program, size_t stack_capacity,
                           int32_t **stack_bottom);

struct cpu *cpu_create(int32_t *memory, int32_t *stack_bottom,
                       size_t stack_capacity);

int32_t cpu_get_register(struct cpu *cpu, enum cpu_register reg);

void cpu_set_register(struct cpu *cpu, enum cpu_register reg, int32_t value);

enum cpu_status cpu_get_status(struct cpu *cpu);

int32_t cpu_get_stack_size(struct cpu *cpu);

void cpu_destroy(struct cpu *cpu);

void cpu_reset(struct cpu *cpu);

int cpu_step(struct cpu *cpu);

long long cpu_run(struct cpu *cpu, size_t steps);

#endif  // CPU_H
