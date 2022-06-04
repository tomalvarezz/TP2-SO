// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifdef BUDDY_MEMORY_MANAGER
#include <memory_manager.h>
#include <buddy_list.h>
#include <libraryc.h>

#define MIN_ALLOC_LOG2 4
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)
#define MAX_ALLOC_LOG2 31
#define MAX_LEVEL_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2)

static list_t levels[MAX_LEVEL_COUNT];
static size_t maximum_size;
static list_t* base_ptr;
static uint8_t levels_amount;

static void add_to_level(list_t* list, list_t* node, uint8_t tree_level);
static size_t get_min_level(size_t request);
static int get_avail_level(uint8_t min_level);
static list_t* get_buddy(list_t* node);
static list_t* get_node_address(list_t* node);
static int log2(uint32_t number);

void initialize_memory_manager(char* heap_base, size_t heap_size) {
    if (heap_base == NULL) {
        return;
    }

    base_ptr = (list_t*)heap_base;

    maximum_size = heap_size;

    levels_amount = (int)log2(heap_size) - MIN_ALLOC_LOG2 + 1;

    if (levels_amount > MAX_LEVEL_COUNT) {
        levels_amount = MAX_LEVEL_COUNT;
    }

    for (int i = 0; i < levels_amount; i++) {
        list_create(&levels[i]);
        levels[i].free = 0;
        levels[i].level = i;
    }

    add_to_level(&levels[levels_amount - 1], base_ptr, levels_amount - 1);
}

void* malloc(uint64_t malloc_bytes) {
    size_t bytes_needed = malloc_bytes + sizeof(list_t);

    if (bytes_needed == 0 || bytes_needed > maximum_size + 1) {
        return NULL;
    }

    uint8_t min_level = get_min_level(bytes_needed);
    int avail_level = get_avail_level(min_level);

    if (avail_level == -1) {
        return NULL;
    }

    list_t* malloc_node = list_pop(&levels[avail_level]);

    while (min_level < avail_level) {
        malloc_node->level--;
        add_to_level(&levels[avail_level - 1], get_buddy(malloc_node), avail_level - 1);
        avail_level--;
    }

    malloc_node->free = 0;

    return (void*)(malloc_node + 1);
}

void free(void* block) {
    if (block == NULL) {
        return;
    }

    list_t* free_node = (list_t*)block - 1;

    free_node->free = 1;

    list_t* free_node_buddy = get_buddy(free_node);

    while (free_node->level != (levels_amount - 1) && free_node_buddy->level == free_node->level && free_node_buddy->free) {
        list_remove(free_node_buddy);
        free_node = get_node_address(free_node);
        free_node->level++;
        free_node_buddy = get_buddy(free_node);
    }

    list_push(&levels[free_node->level], free_node);
}

void memory_dump() {
    printf("\nUtilizando BUDDY_MEMORY_MANAGER\n");
     list_t *list, *listAux;
  uint32_t idx = 0;
  uint32_t spaceAvailable = 0;

  printf("VUELCO DE MEMORIA\n");
  printf("\nNiveles con bloques libres:\n\n");

  for (int i = levels_amount - 1; i >= 0; i--) {
    list = &levels[i];
    if (!is_empty(list)) {
      printf("    Nivel %d\n", i + MIN_ALLOC_LOG2);
      printf("    Bloques de tamanio 2^%d\n", i + MIN_ALLOC_LOG2);
      for (listAux = list->next, idx = 1; listAux != list;
           idx++, listAux = listAux->next) {
        if (listAux->free) {
          printf("        Numero de bloque: %d\n", idx);
          printf("        Estado: free\n\n");
          spaceAvailable += idx * (1 << (MIN_ALLOC_LOG2 + i));
        }
      }
    }
  }
  printf("\nEspacio disponible: %d\n\n", spaceAvailable);
}

static void add_to_level(list_t* list, list_t* node, uint8_t tree_level) {
    node->level = tree_level;
    node->free = 1;
    list_push(list, node);
}

static size_t get_min_level(size_t request) {
    size_t request_log = log2(request);

    if (request_log < MIN_ALLOC_LOG2) {
        return 0;
    }

    request_log -= MIN_ALLOC_LOG2;

    // Se chequea si el request entra justo en un cierto level
    if (request && !(request & (request - 1))) {
        return request_log;
    }

    return request_log + 1;
}

static int get_avail_level(uint8_t min_level) {
    uint8_t avail_level = min_level;

    while (avail_level < levels_amount && is_empty(&levels[avail_level])) {
        avail_level++;
    }

    if (avail_level > levels_amount) {
        return -1;
    }

    return avail_level;
}

static list_t* get_buddy(list_t* node) {
    uint8_t level = node->level;
    uintptr_t current_offset = (uintptr_t)node - (uintptr_t)base_ptr;
    uintptr_t new_offset = current_offset ^ (1 << (MIN_ALLOC_LOG2 + level));

    return (list_t*)((uintptr_t)base_ptr + new_offset);
}

static list_t* get_node_address(list_t* node) {
    uint8_t level = node->level;
    uintptr_t mask = (1 << (MIN_ALLOC_LOG2 + level));
    mask = ~mask;

    uintptr_t current_offset = (uintptr_t)node - (uintptr_t)base_ptr;
    uintptr_t new_offset = current_offset & mask;

    return (list_t*)(new_offset + (uintptr_t)base_ptr);
}

static int log2(uint32_t number) {
    if (number == 0) {
        return -1;
    }

    int result = -1;
    while (number) {
        result++;
        number >>= 1;
    }
    return result;
}
#endif