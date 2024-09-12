#include "ia32_gen.h"

#include "../memory_safety/memory_safety.h"
#include "../logger/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* generate_ia32_code_for_single_instruction(Instruction instruction) {
    if (instruction.type == SPACE) {
        return "";
    }

    char* result = (char*)scalloc(100, sizeof(char));

    switch (instruction.type) {
        case STOP:
            sprintf(result, "mov eax, 1\nmov ebx, 0\nint 80h");
            break;
        case INPUT:
            sprintf(result, "push __input_msg\ncall _reads\ncall _atoi\nmov [__mem_%d], eax\npop edx", instruction.arg1);
            break;
        case OUTPUT:
            sprintf(result, "push __input_msg\npush dword [__mem_%d]\ncall _itoa\npop edx\npush eax\ncall _puts\npop edx\npop edx\n", instruction.arg1);
            break;
        case ADD:
            sprintf(result, "add eax, [__mem_%d]", instruction.arg1);
            break;
        case SUB:
            sprintf(result, "sub eax, [__mem_%d]", instruction.arg1);
            break;
        case MUL:
            sprintf(result, "imul dword [__mem_%d]", instruction.arg1);
            break;
        case DIV:
            sprintf(result, "idiv dword [__mem_%d]", instruction.arg1);
            break;
        case LOAD:
            sprintf(result, "mov eax, [__mem_%d]", instruction.arg1);
            break;
        case STORE:
            sprintf(result, "mov [__mem_%d], eax", instruction.arg1);
            break;
        case JMP:
            sprintf(result, "jmp __label_%d", instruction.arg1);
            break;
        case JMPN:
            sprintf(result, "cmp eax, 0\njl __label_%d", instruction.arg1);
            break;
        case JMPP:
            sprintf(result, "cmp eax, 0\njg __label_%d", instruction.arg1);
            break;
        case JMPZ:
            sprintf(result, "cmp eax, 0\nje __label_%d", instruction.arg1);
            break;
        case COPY:
            sprintf(result, "mov eax, [__mem_%d]\nmov [__mem_%d], eax", instruction.arg1, instruction.arg2);
            break;
    }

    // Overflow check
    if (instruction.type == MUL) {
        sprintf(result, "%s\njo __overflow", result);
    }

    return result;
}

int* find_jumps(Instruction* instructions, int instruction_count, int* out_jump_count) {
    int* jumps = (int*)scalloc(1, sizeof(int));
    int jump_count = 0;

    for (int i = 0; i < instruction_count; i++) {
        if (instructions[i].type == JMP || instructions[i].type == JMPN || instructions[i].type == JMPP || instructions[i].type == JMPZ) {
            jump_count++;
            jumps = (int*)srealloc(jumps, jump_count * sizeof(int));
            jumps[jump_count - 1] = instructions[i].arg1;
        }
    }

    *out_jump_count = jump_count;

    return jumps;
}

int* find_mems(Instruction* instructions, int instruction_count, int* out_mem_count) {
    int* mems = (int*)scalloc(1, sizeof(int));
    int mem_count = 0;

    for (int i = 0; i < instruction_count; i++) {
        log_debug("Looking for mems - Instruction type: %d\n", instructions[i].type);
        if (instructions[i].type == SPACE) {
            mem_count++;
            mems = (int*)srealloc(mems, mem_count * sizeof(int));
            mems[mem_count - 1] = instructions[i].arg2;
        }
    }

    *out_mem_count = mem_count;

    return mems;
}

char* generate_ia32_code(Instruction* instructions, int instruction_count, int* original_program) {
    char* result = (char*)scalloc(1000, sizeof(char));

    // generate data section
    int total_mems;
    int* mems = find_mems(instructions, instruction_count, &total_mems);
    log_debug("Found %d mems\n", total_mems);

    sprintf(result, "section .data\n");
    strcat(result, "overflow_msg db 'Overflow detected', 0\n");
    for (int i = 0; i < total_mems; i++) {
        char mem[100];
        int value = original_program[mems[i]];
        sprintf(mem, "__mem_%d dd %d", mems[i], value);
        strcat(result, mem);
        strcat(result, "\n");
        log_debug("Generated mem %s\n", mem);
    }

    // generate input message in bss section
    strcat(result, "section .bss\n");
    strcat(result, "__input_msg resb 100\n");

    // generate text section
    strcat(result, "section .text\n");
    strcat(result, "global _start\n");
    strcat(result, "_start:\n");

    int total_jumps;
    int* jumps = find_jumps(instructions, instruction_count, &total_jumps);
    int current_mem = 0;

    for (int i = 0; i < instruction_count; i++) {
        // Add the label if needed
        for (int j = 0; j < total_jumps; j++) {
            if (jumps[j] == current_mem) {
                strcat(result, "__label_");
                char label[10];
                sprintf(label, "%d:\n", current_mem);
                strcat(result, label);
            }
        }

        // Generate the corresponding instruction
        char* instruction_code = generate_ia32_code_for_single_instruction(instructions[i]);
        strcat(result, instruction_code);
        strcat(result, "\n");

        current_mem += get_arg_count(instructions[i].type) + 1;
    }

    // add overflow label
    // print an error from 
    strcat(result, "__overflow:\n");
    strcat(result, "mov eax, 4\n");
    strcat(result, "mov ebx, 1\n");
    strcat(result, "mov ecx, overflow_msg\n");
    strcat(result, "mov edx, 17\n");
    strcat(result, "int 80h\n");
    strcat(result, "mov eax, 1\n");
    strcat(result, "mov ebx, 0\n");
    strcat(result, "int 80h\n");

    return result;
}