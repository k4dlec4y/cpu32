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
    enum cpu_status status;
    int32_t *memory;
    int32_t instruction_index;
    int32_t arithmetic_regs[4];

    int8_t has_stack;
    size_t stack_size;
    int32_t *stack_bottom;
    int32_t *stack_top;
    /* stack roof is the lowest valid stack adress (closest to instructions) */
    int32_t *stack_roof;
};

/**
 * @brief Allocates memory for both instructions and stack.
 * It also copies program (instructions) into the memory.
 * 
 * The memory is allocated in 4 KiB blocks. Instructions are stored at the start
 * of the memory, and the stack is placed after them. The stack grows downward
 * when pushing (stack bottom is the highest address). The address space
 * between the end of instructions and start (top) of the stack is filled
 * with zeros.
 * 
 * @param program        file handler containing the program to be executed
 * @param stack_capacity desired stack size, count of int32_t cells, not bytes
 * @param stack_bottom   out parameter, where stack bottom is stored
 * 
 * @return pointer to the memory, NULL in case of error
 * 
 * @note If the program size is not divisible by 4 (sizeof int32_t), it is
 * considered as an error.
 */
int32_t *cpu_create_memory(FILE *program, size_t stack_capacity,
                           int32_t **stack_bottom);

/**
 * @brief Allocates and initializes struct cpu.
 * 
 * @param memory         pointer to the memory created by cpu_create_memory()
 * @param stack_capacity
 * @param stack_bottom   pointer to the stack bottom
 * 
 * @return pointer to the struct cpu, NULL in case of error
 */
struct cpu *cpu_create(int32_t *memory, int32_t *stack_bottom,
                       size_t stack_capacity);

int32_t cpu_get_register(struct cpu *cpu, enum cpu_register reg);

void cpu_set_register(struct cpu *cpu, enum cpu_register reg, int32_t value);

enum cpu_status cpu_get_status(struct cpu *cpu);

int32_t cpu_get_stack_size(struct cpu *cpu);

/**
 * @brief Sets registers/pointers to 0/NULL and releases resources (memory)
 * 
 * @param cpu pointer to the cpu
 */
void cpu_destroy(struct cpu *cpu);

/**
 * @brief Zeroes out registers (status included) and stack. Doesn't deallocate
 * any memory.
 * 
 * @param cpu pointer to the cpu
 */
void cpu_reset(struct cpu *cpu);

/**
 * @brief Executes one instruction.
 * 
 * @param cpu pointer to the cpu
 * 
 * @return non-zero if the instruction succeeded, 0 in case of error
 */
int cpu_step(struct cpu *cpu);

/**
 * @brief Executes `steps` instructions.
 * 
 * @return if the cpu reaches an error status while executing K-th step,
 * returns -K; otherwise it returns the real count of executed instructions
 * 
 * @note the real count can be lower than steps, if the cpu executes halt
 */
long long cpu_run(struct cpu *cpu, size_t steps);

#endif  // CPU_H
