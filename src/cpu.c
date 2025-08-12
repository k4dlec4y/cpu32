#include "../include/cpu.h"
#include "../include/instructions.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const size_t BLOCK_4KB = 4096;

static int32_t* memory_increase(int32_t *memory, size_t size, size_t increase)
{
    int32_t* temp_p = realloc(memory, size + increase);

    if (temp_p == NULL)
    {
        free(memory);
        return NULL;
    }

    memset(temp_p + size / 4, 0, increase);
    
    return temp_p;
}

int32_t* cpu_create_memory(FILE *program, size_t stack_capacity, int32_t **stack_bottom)
{
    assert(program != NULL);
    assert(stack_bottom != NULL);

    size_t size = BLOCK_4KB;

    // calloc to set nulls
    int32_t* memory = calloc(size, 1);
    if (memory == NULL)
    {
        return NULL;
    }

    int32_t c = fgetc(program);
    size_t cur_size = 0;
    int32_t number = 0;

    while (c != EOF)
    {
        for (size_t i = 0; i < 4; i++)
        {
            cur_size += 1;
            if (cur_size >= size)
            {
                memory = memory_increase(memory, size, BLOCK_4KB);
                if (memory == NULL)
                {
                    return NULL;
                }
                size += BLOCK_4KB;
            }

            // this checks if cells size are int32_t size
            if (c == EOF)
            {
                free(memory);
                return NULL;
            }
            number = number | (c << (i * 8));
            c = fgetc(program);
        }
        *(memory + (cur_size - 4) / 4) = number;
        number = 0;
    }
    
    int count = ((cur_size + stack_capacity * 4) / BLOCK_4KB) - (size / BLOCK_4KB);
    count += ((cur_size + stack_capacity * 4) % BLOCK_4KB != 0) ? 1 : 0;
    if (count > 0)
    {
        memory = memory_increase(memory, size, count * BLOCK_4KB);
        if (memory == NULL)
        {
            return NULL;
        }
        size += count * BLOCK_4KB;
    }

    *stack_bottom = (memory + size / 4 - 1);
    return memory;

}

struct cpu *cpu_create(int32_t *memory, int32_t *stack_bottom, size_t stack_capacity)
{
    assert(memory != NULL);
    assert(stack_bottom != NULL);

    struct cpu* cpu = malloc(sizeof(struct cpu));
    if (cpu == NULL)
    {
        return NULL;
    }
    cpu->arithmetic_regs[REGISTER_A] = 0;
    cpu->arithmetic_regs[REGISTER_B] = 0;
    cpu->arithmetic_regs[REGISTER_C] = 0;
    cpu->arithmetic_regs[REGISTER_D] = 0;
    cpu->instruction_index = 0;
    cpu->mem_start_address = memory;

    cpu->stack_size = 0;
    cpu->stack_top = stack_bottom;
    cpu->stack_bottom = stack_bottom;
    cpu->stack_roof = stack_bottom - stack_capacity + 1;
    cpu->status = CPU_OK;
    cpu->is_neg = 0;
    cpu->has_stack = (stack_capacity > 0) ? 1 : 0;
    return cpu;
}

int32_t cpu_get_register(struct cpu *cpu, enum cpu_register reg)
{
    assert(cpu != NULL);
    assert(reg >= REGISTER_A && reg <= REGISTER_D);
    return cpu->arithmetic_regs[reg];
}

void cpu_set_register(struct cpu *cpu, enum cpu_register reg, int32_t value)
{
    assert(cpu != NULL);
    assert(reg >= REGISTER_A && reg <= REGISTER_D);
    cpu->arithmetic_regs[reg] = value;
}

enum cpu_status cpu_get_status(struct cpu *cpu)
{
    assert(cpu != NULL);
    return cpu->status;
}

int32_t cpu_get_stack_size(struct cpu *cpu)
{
    assert(cpu != NULL);
    return cpu->stack_size;
}

void cpu_reset_aux(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->arithmetic_regs[REGISTER_A] = 0;
    cpu->arithmetic_regs[REGISTER_B] = 0;
    cpu->arithmetic_regs[REGISTER_C] = 0;
    cpu->arithmetic_regs[REGISTER_D] = 0;

    cpu->stack_size = 0;
    cpu->instruction_index = 0;
}

void cpu_destroy(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu_reset_aux(cpu);

    free(cpu->mem_start_address);
    cpu->mem_start_address = NULL;

    cpu->stack_top = NULL;
    cpu->stack_bottom = NULL;
    cpu->stack_roof = NULL;
    cpu->status = 0;
    cpu->is_neg = 0;
    cpu->has_stack = 0;
}

void cpu_reset(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu_reset_aux(cpu);

    cpu->status = CPU_OK;
    memset(cpu->stack_roof, 0, (cpu->stack_bottom - cpu->stack_roof) * 4);
    cpu->stack_top = cpu->stack_bottom;
}

int cpu_step(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->status != CPU_OK)
    {
        return 0;
    }

    int32_t* memory_pointer = cpu->mem_start_address + cpu->instruction_index;
    if (memory_pointer >= cpu->stack_roof || cpu->instruction_index < 0)
    {
        cpu->status = CPU_INVALID_ADDRESS;
        return 0;
    }

    int32_t instruction_number = *(memory_pointer);
    if (instruction_number < 0 || instruction_number > 18)
    {
        cpu->status = CPU_ILLEGAL_INSTRUCTION;
        return 0;
    }

    return instructions[instruction_number](cpu);
}

long long cpu_run(struct cpu *cpu, size_t steps)
{
    assert(cpu != NULL);
    if (cpu->status != CPU_OK)
    {
        return 0;
    }
    size_t i = 1;
    while (i <= steps)
    {
        if (cpu_step(cpu) == 0)
        {
            if (cpu->status == CPU_HALTED)
            {
                return i;
            }
            return -i;
        }
        i++;
    }
    return i - 1;
}
