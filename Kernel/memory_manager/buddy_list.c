// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <buddy_list.h>

void list_create(list_t* list) {
    list->prev = list;
    list->next = list;
}

void list_push(list_t* list, list_t* to_add) {
    list_t* prev = list->prev;

    to_add->prev = prev;
    to_add->next = list;

    prev->next = to_add;
    list->prev = to_add;
}

void list_remove(list_t* to_remove) {
    list_t* prev = to_remove->prev;
    list_t* next = to_remove->next;

    prev->next = next;
    next->prev = prev;
}

list_t* list_pop(list_t* list) {
    list_t* to_ret = list->prev;
    if (to_ret == list) {
        return NULL;
    }
    list_remove(to_ret);
    return to_ret;
}

char is_empty(list_t* list) { return list->prev == list; }
