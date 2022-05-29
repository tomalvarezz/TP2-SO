#ifndef BUDDY_LIST_H
#define BUDDY_LIST_H

#include <stdint.h>
#include <stddef.h>

typedef struct list_t {
    uint8_t level;
    uint8_t free;
    struct list_t *prev, *next;
} list_t;

void list_create(list_t* list);
void list_push(list_t* list, list_t* to_add);
void list_remove(list_t* to_remove);
list_t* list_pop(list_t* list);
char is_empty(list_t* list);

#endif