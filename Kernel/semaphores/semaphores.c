#include <lib.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <semaphores.h>

#define ERROR -1
#define OK 0

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

t_semaphore* semaphores;

static t_semaphore* get_semaphore(uint32_t id);
static t_semaphore* new_semaphore(uint32_t id, uint64_t initialValue);
static void add_to_list(t_semaphore* newSem);
static void unblock_process(t_semaphore* sem);
static void remove_from_list(t_semaphore* sem);
static void blocked_processes_dump(int* blocked_processes, uint16_t blocked_processes_count);

int sem_open(uint32_t id, uint64_t initial_value) {
    t_semaphore* semaphore = get_semaphore(id);

    if (initial_value < 0) {
        return ERROR;
    }

    if (semaphore == NULL) {
        semaphore = new_semaphore(id, initial_value);
        if (semaphore == NULL) {
            return ERROR;
        }
    }

    if (semaphore->listening_processes >= MAX_BLOCKED_PROCESSES) {
        return ERROR;
    }

    semaphore->listening_processes++;
    return id;
}

int sem_wait(uint32_t id) {
    t_semaphore* sem;
    if ((sem = get_semaphore(id)) == NULL) {
        return ERROR;
    }

    acquire(&(sem->lock));

    if (sem->value > 0) {
        sem->value--;
        release(&(sem->lock));
    } else {
        int currentPID = get_process_PID();
        sem->blocked_processes[sem->blocked_processes_count++] = currentPID;
        release(&(sem->lock));
        block_process(currentPID);
    }
    return OK;
}

int sem_post(uint32_t id) {
    t_semaphore* sem;
    if ((sem = get_semaphore(id)) == NULL) {
        return ERROR;
    }

    acquire(&(sem->lock));
    if (sem->blocked_processes_count > 0) {
        unblock_process(sem);
    } else {
        sem->value++;
    }

    release(&(sem->lock));
    return OK;
}

int sem_close(uint32_t id) {
    t_semaphore* sem;
    if ((sem = get_semaphore(id)) == NULL) {
        return ERROR;
    }

    if (sem->listening_processes > 0) {
        sem->listening_processes--;
    }

    if (sem->listening_processes == 0) {
        remove_from_list(sem);
        free(sem);
    }

    return OK;
}

void sem_status() {
    printf("\nEstado de los semaforos activos\n\n");
    t_semaphore* sem = semaphores;
    while (sem) {
        printf("ID: %d\n", sem->id);
        printf("\tValor: %d\n", sem->value);
        printf("\tCantidad de procesos ligados al semaforo: %d\n", sem->listening_processes);
        printf("\tCantidad de procesos bloqueados: %d\n", sem->blocked_processes_count);
        printf("\tProcesos blockeados:\n");
        blocked_processes_dump(sem->blocked_processes, sem->blocked_processes_count);
        sem = sem->next;
    }
}

static void blocked_processes_dump(int* blocked_processes, uint16_t blocked_processes_count) {
    for (int i = 0; i < blocked_processes_count; i++) {
        printf("\tPID: %d\n", blocked_processes[i]);
    }
    printf("\n");
}

static t_semaphore* new_semaphore(uint32_t id, uint64_t initial_value) {
    t_semaphore* new = malloc(sizeof(t_semaphore));
    if (new != NULL) {
        new->id = id;
        new->value = initial_value;
        new->blocked_processes_count = 0;
        new->listening_processes = 0;
        new->lock = 0;
        new->next = NULL;
        add_to_list(new);
    }
    return new;
}

static void add_to_list(t_semaphore* new) {
    t_semaphore* tail_sem = semaphores;
    if (tail_sem == NULL) {
        semaphores = new;
    } else {
        while (tail_sem->next != NULL) {
            tail_sem = tail_sem->next;
        }
        tail_sem->next = new;
    }
}

static t_semaphore* get_semaphore(uint32_t id) {
    t_semaphore* result = semaphores;
    while (result) {
        if (result->id == id) {
            return result;
        }
        result = result->next;
    }
    return NULL;
}

static void unblock_process(t_semaphore* sem) {
    int ready_PID = sem->blocked_processes[0];

    for (int i = 0; i < sem->blocked_processes_count - 1; i++) {
        sem->blocked_processes[i] = sem->blocked_processes[i + 1];
    }

    sem->blocked_processes_count--;
    ready_process(ready_PID);
}

static void remove_from_list(t_semaphore* sem) {
    t_semaphore* sem_aux = semaphores;
    if (sem == sem_aux) {
        semaphores = sem_aux->next;
    } else {
        while (sem_aux->next != sem) {
            sem_aux = sem_aux->next;
        }
        sem_aux->next = sem->next;
    }
    free(sem);
}
