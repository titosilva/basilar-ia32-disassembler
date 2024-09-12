#include "parser.h"
#include "../memory_safety/memory_safety.h"
#include "../instructions/instructions.h"
#include "../logger/logger.h"

#include <stdlib.h>

int find_last_stop(int *program, int program_length) {
    for (int i = program_length - 1; i > 0; i--) {
        if (program[i] == STOP) {
            return i;
        }
    }

    return -1;
}

Instruction* parse_instructions(int* program, int* out_instruction_count) {
    Instruction* result = (Instruction*)smalloc(sizeof(Instruction));

    int i = 0;
    while (program[i] != -1) {
        i++;
    }

    *out_instruction_count = 0;
    int last_stop = find_last_stop(program, i);
    i = 0;
    while (program[i] != -1) {
        log_debug("Parsing instruction %d\n", i);
        log_debug("Instruction type: %d\n", program[i]);

        result = (Instruction*)srealloc(result, (i + 1) * sizeof(Instruction));

        result[*out_instruction_count].type = program[i];
        int args = get_arg_count(result[*out_instruction_count].type);

        if (args == -1 || i > last_stop) {
            result[*out_instruction_count].type = SPACE;
            result[*out_instruction_count].arg1 = program[i];
            result[*out_instruction_count].arg2 = i;
            args = 0;
        }

        log_debug("Instruction has %d args\n", args);

        if (args > 0) {
            result[*out_instruction_count].arg1 = program[i + 1];
        }

        if (args > 1) {
            result[*out_instruction_count].arg2 = program[i + 2];
        }

        log_debug("Generated instruction %d %d %d\n", result[*out_instruction_count].type, result[*out_instruction_count].arg1, result[*out_instruction_count].arg2);

        i += args + 1;
        *out_instruction_count = *out_instruction_count + 1;
    }

    return result;
}