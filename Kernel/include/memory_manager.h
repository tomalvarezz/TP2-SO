#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>

void initialize_memory_manager(char *heap_base, uint64_t heap_size);
void *malloc(uint64_t malloc_bytes);
void free(void *block);
#endif