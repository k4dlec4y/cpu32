#include "../include/instructions.h"
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

int nop(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->instruction_index++;
    return 1;
}

int halt(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->instruction_index++;
    cpu->status = CPU_HALTED;
    return 0;
}

int add(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[REGISTER_A] += cpu->arithmetic_regs[reg];
    cpu->instruction_index += 2;
    return 1;
}

int sub(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[REGISTER_A] -= cpu->arithmetic_regs[reg];
    cpu->instruction_index += 2;
    return 1;
}

int mul(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[REGISTER_A] *= cpu->arithmetic_regs[reg];
    cpu->instruction_index += 2;
    return 1;
}

int div0(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    if (cpu->arithmetic_regs[reg] == 0)
    {
        cpu->status = CPU_DIV_BY_ZERO;
        return 0;
    }
    cpu->arithmetic_regs[REGISTER_A] /= cpu->arithmetic_regs[reg];
    cpu->instruction_index += 2;
    return 1;
}

int inc(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[reg]++;
    cpu->instruction_index += 2;
    return 1;
}

int dec(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[reg]--;
    cpu->instruction_index += 2;
    return 1;
}

int loop(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t index = *(memory_pointer + 1);

    if (cpu->arithmetic_regs[REGISTER_C] != 0)
    {
        cpu->instruction_index = index;
        return 1;
    }
    cpu->instruction_index += 2;
    return 1;
}

int movr(struct cpu *cpu)
{
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t number = *(memory_pointer + 2);
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[reg] = number;
    cpu->instruction_index += 3;
    return 1;
}

int load(struct cpu *cpu)
{
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t number = *(memory_pointer + 2);
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int32_t* pointer = cpu->stack_top + cpu->arithmetic_regs[REGISTER_D] + number;

    if (check_stack(cpu, pointer, number) == 1)
    {
        return 0;
    }

    cpu->arithmetic_regs[reg] = *pointer;
    cpu->instruction_index += 3;
    return 1;
}

int store(struct cpu *cpu)
{
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t number = *(memory_pointer + 2);
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int32_t* pointer = cpu->stack_top + cpu->arithmetic_regs[REGISTER_D] + number;

    if (check_stack(cpu, pointer, number) == 1)
    {
        return 0;
    }

    *(pointer) = cpu->arithmetic_regs[reg];
    cpu->instruction_index += 3;
    return 1;
}

int in(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int32_t number;
    int ret_val = scanf("%" SCNd32, &number);

    if (ret_val == 0)
    {
        cpu->status = CPU_IO_ERROR;
        return 0;
    }

    if (ret_val == EOF)
    {
        cpu->arithmetic_regs[REGISTER_C] = 0;
        cpu->arithmetic_regs[reg] = -1;
    }
    else
    {
        cpu->arithmetic_regs[reg] = number;
    }

    cpu->instruction_index += 2;
    return 1;
}

int get(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int ch = getchar();
    if (ch == EOF)
    {
        cpu->arithmetic_regs[REGISTER_C] = 0;
        cpu->arithmetic_regs[reg] = -1;
    }
    else
    {
        cpu->arithmetic_regs[reg] = ch;
    }
    cpu->instruction_index += 2;
    return 1;
}

int out(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int32_t number = cpu->arithmetic_regs[reg];
    printf("%" SCNd32, number);
    cpu->instruction_index += 2;
    return 1;
}

int put(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    int32_t number = cpu->arithmetic_regs[reg];
    if (number < 0 || number > UCHAR_MAX)
    {
        cpu->status = CPU_ILLEGAL_OPERAND;
        return 0;
    }
    putchar(number);
    cpu->instruction_index += 2;
    return 1;
}

int swap(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg1 = *(memory_pointer + 1);
    int32_t reg2 = *(memory_pointer + 2);
    if (check_reg(cpu, reg1) == 1 || check_reg(cpu, reg2) == 1)
    {
        return 0;
    }

    int32_t temp = cpu->arithmetic_regs[reg1];
    cpu->arithmetic_regs[reg1] = cpu->arithmetic_regs[reg2];
    cpu->arithmetic_regs[reg2] = temp;
    cpu->instruction_index += 3;
    return 1;
}

int push(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    // better to use cpu->stack_bottom - cpu->stack_size
    // cpu_stack_top's value for stack_size 0 and 1 is the same
    if (cpu->stack_bottom - cpu->stack_size < cpu->stack_roof)
    {
        cpu->status = CPU_INVALID_STACK_OPERATION;
        return 0;
    }

    if (cpu->stack_size != 0)
    {
        cpu->stack_top--;
    }
    *(cpu->stack_top) = cpu->arithmetic_regs[reg];
    cpu->stack_size++;
    cpu->instruction_index += 2;
    return 1;
}

int pop(struct cpu *cpu)
{
    assert(cpu != NULL);
    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    int32_t reg = *(memory_pointer + 1);
    if (check_reg(cpu, reg) == 1)
    {
        return 0;
    }

    if (cpu->stack_size == 0)
    {
        cpu->status = CPU_INVALID_STACK_OPERATION;
        return 0;
    }

    cpu->arithmetic_regs[reg] = *(cpu->stack_top);
    *(cpu->stack_top) = 0;
    if (cpu->stack_size > 1)
    {
        cpu->stack_top++;
    }
    cpu->stack_size--;
    cpu->instruction_index += 2;
    return 1;
}
