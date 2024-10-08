#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "src/memory_safety/memory_safety.h"
#include "src/parser/parser.h"
#include "src/ia32_gen/ia32_gen.h"
#include "src/instructions/instructions.h"
#include "src/logger/logger.h"

int* read_numbers(FILE* file) {
    int i = 0;

    int* numbers = (int*)smalloc(sizeof(int));
    while (fscanf(file, "%d", &numbers[i]) != EOF) {
        i++;
        numbers = (int*)srealloc(numbers, (i + 1) * sizeof(int));
    }

    numbers[i] = -1;
    return numbers;
}

char * change_file_extension(char * file_name, char * new_extension) {
    char * result = (char *)smalloc(100 * sizeof(char));
    
    int i = 0;
    while (file_name[i] != '.') {
        result[i] = file_name[i];
        i++;
    }

    result[i] = '\0';
    strcat(result, new_extension);
    return result;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    init_alloc_table();

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    int* numbers = read_numbers(file);
    fclose(file);
    log_debug("Read numbers from file\n");

    log_debug("Parsing instructions\n");
    int instruction_count;
    Instruction* instructions = parse_instructions(numbers, &instruction_count);
    log_debug("Parsed instructions\n");

    char* code = generate_ia32_code(instructions, instruction_count, numbers);
    log_debug("Generated code\n");

    char * out_file_name = change_file_extension(argv[1], ".s");
    log_debug("Writing code to %s\n", out_file_name);
    FILE* out_file = fopen(out_file_name, "w");
    if (out_file == NULL) {
        printf("Could not open file out.asm\n");
        return 1;
    }

    FILE* io_file = fopen("io.asm", "r");
    if (io_file == NULL) {
        printf("Could not open file io.asm\n");
        return 1;
    }

    char io_code[1000];
    while (fgets(io_code, 1000, io_file) != NULL) {
        fprintf(out_file, "%s", io_code);
    }

    fprintf(out_file, "\n\n%s", code);
    fclose(out_file);
    log_debug("Done\n");

    end_alloc_table();
}