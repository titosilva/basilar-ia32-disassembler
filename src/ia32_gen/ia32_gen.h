#pragma once

#include "../instructions/instructions.h"

char* generate_ia32_code(Instruction* instructions, int instruction_count, int* original_program);