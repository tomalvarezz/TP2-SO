#ifdef KR_MEMORY_MANAGER
#include <stddef.h>
#include <stdint.h>
#include <libraryc.h>

#define TRUE 1
#define FALSE !TRUE

typedef long Align;

typedef union header {
    struct {
        union header* ptr;
        uint64_t size;
    } data;

    Align x;

} header;

static header* base;
static header* free_node = NULL;

uint64_t total_heap_size;

void initialize_memory_manager(char* heap_base, uint64_t heap_size) {
    if (heap_base == NULL) {
        return;
    }

    total_heap_size = (heap_size + sizeof(header) - 1) / sizeof(header) + 1;

    free_node = base = (header*)heap_base;

    free_node->data.size = total_heap_size;

    free_node->data.ptr = free_node;
}

void* malloc(uint64_t malloc_bytes) {
    if (malloc_bytes == 0) {
        return NULL;
    }

    header *current_node, *prevptr;

    void* result;

    char node_found;

    uint64_t malloc_units = (malloc_bytes + sizeof(header) - 1) / sizeof(header) + 1;

    prevptr = free_node;

    node_found = TRUE;

    for (current_node = prevptr->data.ptr; node_found; current_node=current_node->data.ptr) {
        if (current_node->data.size >= malloc_units) {
            if (current_node->data.size == malloc_units) {
                prevptr->data.ptr = current_node->data.ptr;
            } else {
                current_node->data.size -= malloc_units;
                current_node += current_node->data.size;
                current_node->data.size = malloc_units;
            }

            free_node = prevptr;

            result = current_node + 1;

            node_found = FALSE;

        }
        if (current_node == free_node) {
            return NULL;
        }
        prevptr = current_node;
    }
    return result;
}

void free(void* block) {
    if (block == NULL) {
        return;
    }

    header *free_block, *current_node;
    free_block = (header*)block - 1;

    if (free_block < base || free_block >= (base + total_heap_size * sizeof(header))) {
        return;
    }

    block = NULL;

    char external = FALSE;

    for (current_node = free_node; !(free_block > current_node && free_block < current_node->data.ptr) && !external; current_node = current_node->data.ptr) {
        if (free_block == current_node || free_block == current_node->data.ptr) {
            return;
        }

        if (current_node >= current_node->data.ptr && (free_block > current_node || free_block < current_node->data.ptr)) {
            external = TRUE;
        }
    }

    if (!external && (current_node + current_node->data.size > free_block || free_block + free_block->data.size > current_node->data.ptr)) {
        return;
    }

    if (free_block + free_block->data.size == current_node->data.ptr) {
        free_block->data.size += current_node->data.ptr->data.size;
        free_block->data.ptr = current_node->data.ptr->data.ptr;
    } else {
        free_block->data.ptr = current_node->data.ptr;
    }

    if (current_node + current_node->data.size == free_block) {
        current_node->data.size += free_block->data.size;
        current_node->data.ptr = free_block->data.ptr;
    } else {
        current_node->data.ptr = free_block;
    }

    free_node = current_node;
}

void memory_dump() {
    int block_number = 1;
    header* first;
    header* current;
    first = current = free_node;

    char flag = TRUE;
    printf("\nUtilizando KR_MEMORY_MANAGER\n");
    printf("VUELCO DE MEMORIA \n");
    printf("\nMemoria Total: %d bytes\n\n", (uint32_t)total_heap_size * sizeof(header));

    if (free_node == NULL) {
        printf("\nNo hay bloques de memoria disponibles.\n");
        return;
    }
    printf("Bloques libres:\n\n");

    while (current != first || flag) {
        flag = FALSE;
        printf("\tBloque numero: %d\n", block_number);
        printf("\tBase:%x\n", (uint64_t)current);
        printf("\tBytes disponibles: %d\n\n", (int)current->data.size);

        current = current->data.ptr;
        block_number++;
    }

    printf("\n");
}
#endif