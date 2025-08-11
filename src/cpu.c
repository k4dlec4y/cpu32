#include "cpu.h"
#include <ctype.h>
#include <limits.h>
#include <inttypes.h>

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

const size_t BLOCK_4KB = 4096;

static int nop(struct cpu *cpu);

static int halt(struct cpu *cpu);

static int add(struct cpu *cpu);

static int sub(struct cpu *cpu);

static int mul(struct cpu *cpu);

static int div_ins(struct cpu *cpu);

static int inc(struct cpu *cpu);

static int dec(struct cpu *cpu);

static int loop(struct cpu *cpu);

static int movr(struct cpu *cpu);

static int load(struct cpu *cpu);

static int store(struct cpu *cpu);

static int in(struct cpu *cpu);

static int get(struct cpu *cpu);

static int out(struct cpu *cpu);

static int put(struct cpu *cpu);

static int swap(struct cpu *cpu);

static int push(struct cpu *cpu);

static int pop(struct cpu *cpu);

int (*instructions[19]) (struct cpu *) = 
{
    &nop, &halt, &add, &sub, &mul, &div_ins, &inc, &dec, &loop, &movr, &load, &store, &in, &get, &out, &put, &swap, &push, &pop
};

int check_reg(struct cpu *cpu, enum cpu_register reg)
{
    if (reg < REGISTER_A || reg > REGISTER_D)
    {
        cpu->status = CPU_ILLEGAL_OPERAND;
        return 1;
    }
    return 0;
}

static int check_stack(struct cpu *cpu, int32_t* pointer, int32_t number)
{
    if (cpu->has_stack == 0 ||
        cpu->stack_size == 0 ||
        pointer > cpu->stack_bottom ||
        cpu->arithmetic_regs[REGISTER_D] + number < 0)
    {
        cpu->status = CPU_INVALID_STACK_OPERATION;
        return 1;
    }
    return 0;
}

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

//                         //
//      INSTRUCTIONS       //
//                         //

static int nop(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->instruction_index++;
    return 1;
}

static int halt(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->instruction_index++;
    cpu->status = CPU_HALTED;
    return 0;
}

static int add(struct cpu *cpu)
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

static int sub(struct cpu *cpu)
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

static int mul(struct cpu *cpu)
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

static int div_ins(struct cpu *cpu)
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

static int inc(struct cpu *cpu)
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

static int dec(struct cpu *cpu)
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

static int loop(struct cpu *cpu)
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

static int movr(struct cpu *cpu)
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

static int load(struct cpu *cpu)
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

static int store(struct cpu *cpu)
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

static int in(struct cpu *cpu)
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

static int get(struct cpu *cpu)
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

static int out(struct cpu *cpu)
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

static int put(struct cpu *cpu)
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

static int swap(struct cpu *cpu)
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

static int push(struct cpu *cpu)
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

static int pop(struct cpu *cpu)
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
