#pragma once

#include <inttypes.h>

typedef enum {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    JMP = 5,
    JMPN = 6,
    JMPP = 7,
    JMPZ = 8,
    COPY = 9,
    LOAD = 10,
    STORE = 11,
    INPUT = 12,
    OUTPUT = 13,
    STOP = 14,

    // Space
    SPACE = 99,
} InstructionType;

typedef struct {
    InstructionType type;
    int32_t arg1;
    int32_t arg2;
} Instruction;

int get_arg_count(InstructionType type);