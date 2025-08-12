#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/**
 * @file instructions.h
 * @brief This file contains all available (valid) instructions.
 *
 * In a program, they are represented by 32-bit little-endian numbers. This also
 * applies to operands like REG, INDEX and NUM.
 * 
 * All instructions return 1 on success (halt cannot succeed),
 * and 0 if they were not executed properly.
 */

#include "cpu.h"

/**
 * @brief Instruction 0 - nop
 *
 * This instruction doesn't do anything (only increases program counter).
 */
int nop(struct cpu *cpu);

/**
 * @brief Instruction 1 - halt
 *
 * Stops the execution of the program and sets cpu
 * status to CPU_HALTED. cpu_step() returns 0.
 */
int halt(struct cpu *cpu);

/**
 * @brief Instruction 2 - add REG
 *
 * Adds the value of REG to register A.
 */
int add(struct cpu *cpu);

/**
 * @brief Instruction 3 - sub REG
 *
 * Subtracts the value of REG from register A.
 */
int sub(struct cpu *cpu);

/**
 * @brief Instruction 4 - mul REG
 *
 * Multiplicate register A by the value of REG.
 */
int mul(struct cpu *cpu);

/**
 * @brief Instruction 5 - div REG
 *
 * Register A is divided by the value of REG.
 *
 * If REG contains 0, instruction won't execute and cpu status
 * is set to CPU_DIV_BY_ZERO.
 */
int div0(struct cpu *cpu);

/**
 * @brief Instruction 6 - inc REG
 *
 * Increments the value of REG by 1.
 */
int inc(struct cpu *cpu);

/**
 * @brief Instruction 7 - dec REG
 *
 * Decrements the value of REG by 1.
 */
int dec(struct cpu *cpu);

/**
 * @brief Instruction 8 - loop INDEX
 *
 * If the value of register C is non-zero, program counter jumps
 * on cpu_memory + INDEX.
 */
int loop(struct cpu *cpu);

/**
 * @brief Instruction 9 - movr REG NUM
 *
 * Moves NUM into register REG.
 */
int movr(struct cpu *cpu);

/**
 * @brief Instruction 10 - load REG NUM
 *
 * Loads into REG number located at STACK_TOP + register D + NUM.
 *
 * If the address is located beyond currently filled stack address space,
 * instruction won't execute and cpu status
 * is set to CPU_INVALID_STACK_OPERATION.
 */
int load(struct cpu *cpu);

/**
 * @brief Instruction 11 - store REG NUM
 *
 * Store the value from REG to address STACK_TOP + register D + NUM.
 *
 * If the address is located beyond currently filled stack address space,
 * instruction won't execute and cpu status
 * is set to CPU_INVALID_STACK_OPERATION.
 */
int store(struct cpu *cpu);

/**
 * @brief Instruction 12 - in REG
 *
 * Reads from the input a 32-bit number and stores it into REG.
 *
 * If the value from the input doesn't represent a valid number,
 * instruction won't execute and cpu status is set to CPU_IO_ERROR.
 *
 * If there are no more numbers on the input (EOF), register C is set to 0 and
 * the value of REG is set to -1 (even if REG is C).
 */
int in(struct cpu *cpu);

/**
 * @brief Instruction 13 - get REG
 *
 * Reads from the input a single byte and stores it into REG.
 *
 * If there are no more numbers on the input (EOF), register C is set to 0 and
 * the value of REG is set to -1 (even if REG is C).
 */
int get(struct cpu *cpu);

/**
 * @brief Instruction 14 - out REG
 *
 * Prints the value of REG on the output (string made of '0' - '9' only).
 */
int out(struct cpu *cpu);

/**
 * @brief Instruction 15 - put REG
 *
 * If the value of REG is in the range <0, 255>, it will be printed as a single
 * char on the output.
 *
 * Otherwise instruction won't be executed and cpu status
 * is set to CPU_ILLEGAL_OPERAND.
 */
int put(struct cpu *cpu);

/**
 * @brief Instruction 16 - swap REG REG
 *
 * Swaps the values of registers.
 */
int swap(struct cpu *cpu);

/**
 * @brief Instruction 17 - push REG
 *
 * Pushes the value of REG on the stack (if the stack is not full).
 *
 * Otherwise, instruction won't be executed and cpu status
 * is set to CPU_INVALID_STACK_OPERATION.
 */
int push(struct cpu *cpu);

/**
 * @brief Instruction 18 - pop REG
 *
 * If the stack is not empty, a number is popped from the stack top into REG.
 *
 * Otherwise, instruction won't be executed and cpu status
 * is set to CPU_INVALID_STACK_OPERATION.
 */
int pop(struct cpu *cpu);

extern int (*instructions[19]) (struct cpu *);

#endif  // INSTRUCTIONS_H
