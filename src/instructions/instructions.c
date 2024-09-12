#include "instructions.h"

int get_arg_count(InstructionType type) {
    switch (type) {
        case STOP:
            return 0;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case LOAD:
        case STORE:
        case INPUT:
        case OUTPUT:
        case JMP:
        case JMPN:
        case JMPP:
        case JMPZ:
            return 1;
        case COPY:
            return 2;
    }
    
    return -1;
}
