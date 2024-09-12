#include "memory_safety.h"

#include <stdlib.h>

#define ALLOC_TABLE_BLOCK_SIZE 256
void** __alloc_table;
int __alloc_table_capacity;
int __alloc_table_size;

#define WALK(addr, displacement) ((void*)(((char*)(addr)) + (displacement)))
#define MOONWALK(addr, displacement) ((void*)(((char*)(addr)) - (displacement)))

// used internally to verify if an address has been freed with sfree
char __is_freed(void*);
// use internally to find the index of an address in the allocation table
int __get_alloc_table_idx(void*);

void __increase_alloc_table_if_needed();
void __register_address(void* addr);

void init_alloc_table() {
    __alloc_table = (void**)malloc(ALLOC_TABLE_BLOCK_SIZE * sizeof(void*));
    __alloc_table_capacity = ALLOC_TABLE_BLOCK_SIZE;
    __alloc_table_size = 0;
}

void end_alloc_table() {
    for (int i = 0; i < __alloc_table_size; i++) {
        // sfree does not actually free bytes, so we
        // just do free here
        free(__alloc_table[i]);
    }

    free(__alloc_table);
}

void* smalloc(size_t size) {
    void* addr = malloc(size + sizeof(int) + sizeof(char));
    // Reference to the table index
    *((int*)addr) = __alloc_table_size;
    // Reference to wether memory has been called with sfree or not
    *((char*)WALK(addr, sizeof(int))) = 0;

    __register_address(addr);

    return WALK(addr, sizeof(int) + sizeof(char));
}

void* scalloc(int n_members, size_t member_size) {
    void* addr = calloc(n_members * member_size + sizeof(int) + sizeof(char), sizeof(char));
    // Reference to the table index
    *((int*)addr) = __alloc_table_size;
    // Reference to wether memory has been called with sfree or not
    *((char*)WALK(addr, sizeof(int))) = 0;

    __register_address(addr);

    return WALK(addr, sizeof(int) + sizeof(char));
}

void* srealloc(void* addr, size_t size) {
    int idx = __get_alloc_table_idx(addr);
    void* new_addr = realloc(__alloc_table[idx], size + sizeof(int) + sizeof(char));

    __alloc_table[idx] = new_addr;

    return WALK(new_addr, sizeof(int) + sizeof(char));
}

void sfree(void* addr) {
    if (__is_freed(addr)) return;

    // does not actually free, just marks as free
    *((char*)MOONWALK(addr, sizeof(char))) = 1;
}

char __is_freed(void* addr) {
    return *(char*)MOONWALK(addr, sizeof(char));
}

int __get_alloc_table_idx(void* addr) {
    return *(int*)MOONWALK(addr, sizeof(int) + sizeof(char));
}

void __increase_alloc_table_if_needed() {
    if (__alloc_table_size < __alloc_table_capacity) return;

    __alloc_table_capacity += ALLOC_TABLE_BLOCK_SIZE;
    __alloc_table = (void**) realloc(__alloc_table, __alloc_table_capacity * sizeof(void*));
}

void __register_address(void* addr) {
    __increase_alloc_table_if_needed();

    __alloc_table[__alloc_table_size] = addr;
    __alloc_table_size++;
}