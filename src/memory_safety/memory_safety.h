#pragma once

#include <stddef.h>

void init_alloc_table();
void end_alloc_table();

void* smalloc(size_t size);
void* scalloc(int n_members, size_t member_size);
void* srealloc(void* addr, size_t size);
void sfree(void* addr);