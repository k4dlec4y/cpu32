#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../include/cpu.h"

enum run_mode {
    RUN,
    TRACE
};

static void print_status(enum cpu_status status)
{
    switch (status)
    {
    case CPU_OK:
        puts("cpu status: OK");
        break;
    case CPU_HALTED:
        puts("cpu status: HALTED");
        break;
    case CPU_ILLEGAL_INSTRUCTION:
        puts("cpu status: ILLEGAL_INSTRUCTION");
        break;
    case CPU_ILLEGAL_OPERAND:
        puts("cpu status: ILLEGAL_OPERAND");
        break;
    case CPU_INVALID_ADDRESS:
        puts("cpu status: INVALID_ADDRESS");
        break;
    case CPU_INVALID_STACK_OPERATION:
        puts("cpu status: INVALID_STACK_OPERATION");
        break;
    case CPU_DIV_BY_ZERO:
        puts("cpu status: DIV_BY_ZERO");
        break;
    case CPU_IO_ERROR:
        puts("cpu status: CPU_IO_ERROR");
        break;
    default:
        puts("undefined cpu status");
        break;
    }
}

static int run(struct cpu *cpu)
{
    long long executed = 5000;
    while (executed == 5000 && cpu_get_status(cpu) == CPU_OK) {
        executed = cpu_run(cpu, executed);
    }
    enum cpu_status status = cpu_get_status(cpu);
    cpu_destroy(cpu);
    free(cpu); cpu = NULL;
    print_status(status);
    return status == CPU_HALTED ? 0 : -1;
}

static void print_cpu_info(struct cpu *cpu)
{
    printf(
        "A: %d, B: %d, C: %d, D: %d\n",
        cpu_get_register(cpu, REGISTER_A),
        cpu_get_register(cpu, REGISTER_B),
        cpu_get_register(cpu, REGISTER_C),
        cpu_get_register(cpu, REGISTER_D)
    );
    printf("stack size: %d\n", cpu_get_stack_size(cpu));
    print_status(cpu_get_status(cpu));
}

static int trace(struct cpu *cpu)
{
    while (cpu_get_status(cpu) == CPU_OK) {
        print_cpu_info(cpu);
        cpu_step(cpu);
    }
    print_cpu_info(cpu);

    enum cpu_status status = cpu_get_status(cpu);
    cpu_destroy(cpu);
    free(cpu); cpu = NULL;
    return status == CPU_HALTED ? 0 : -1;
}

static inline void usage(void)
{
    puts("Usage: ./build/cpu32 (run|trace) [stack_capacity] FILE");
}

static inline void file_error(const char *file)
{
    printf("Could not open file: %s\n", file);
}

static inline void stack_size(void)
{
    puts("Your desired stack size is out of long's range.");
}

static inline void insufficient_memory(void)
{
    puts("Insufficient memory for allocation.");
}

int main(int argc, const char *argv[])
{
    errno = 0;
    int file_index = 2;
    size_t stack_capacity = 1024;

    switch (argc)
    {
    case 3:
        break;
    case 4:
        file_index = 3;
        stack_capacity = strtoul(argv[2], NULL, 10);
        if (errno == ERANGE) {
            stack_size();
            return -1;
        }
        break;
    default:
        usage();
        return -1;
    }

    enum run_mode mode;
    if (strcmp(argv[1], "run") == 0) {
        mode = RUN;
    } else if (strcmp(argv[1], "trace") == 0) {
        mode = TRACE;
    } else {
        usage();
        return -1;
    }

    const char *file_name = argv[file_index];
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        file_error(file_name);
        return -1;
    }

    int32_t *stack_bottom;
    int32_t *memory = cpu_create_memory(file, stack_capacity, &stack_bottom);
    if (!memory) {
        fclose(file);
        insufficient_memory();
        return -1;
    }
    fclose(file);

    struct cpu *cpu = cpu_create(memory, stack_bottom, stack_capacity);
    if (!cpu) {
        free(memory); memory = NULL;
        insufficient_memory();
        return -1;
    }
    return mode == RUN ? run(cpu) : trace(cpu); 
}
