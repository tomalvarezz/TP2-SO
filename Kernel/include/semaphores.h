#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stddef.h>
#include <stdint.h>

#define MAX_BLOCKED_PROCESSES 20

typedef struct t_semaphore {
    uint32_t id;
    uint64_t value;
    int blocked_processes[MAX_BLOCKED_PROCESSES];
    uint16_t blocked_processes_count;
    uint16_t listening_processes;
    struct t_semaphore* next;
    int lock;
} t_semaphore;

// Para implementar similar a como tenemos list de processes
typedef struct t_semaphore_list {
    t_semaphore* first;
    t_semaphore* last;
} t_semaphore_list;

int sem_open(uint32_t id, uint64_t initial_value);
int sem_wait(uint32_t id);
int sem_post(uint32_t id);
int sem_close(uint32_t id);
void sem_status();

#endif