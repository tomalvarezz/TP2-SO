// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipes.h>
#include <semaphores.h>
#include <libraryc.h>

#define BUFFER_SIZE 1024
#define MAX_PIPES 8
#define EOF -1

#define IN_USE 1
#define EMPTY 0

static int initial_sem = 1000;

typedef struct t_pipe {
    uint64_t id;
    char buffer[BUFFER_SIZE];
    uint64_t step_to_write, step_to_read;
    long total_processes;
    uint64_t write_sem, read_sem;
    uint64_t state;
} t_pipe;

static t_pipe pipes[MAX_PIPES];

static uint64_t create_pipe(uint64_t id);
static uint64_t get_new_index();
static uint64_t get_index(uint64_t id);

uint64_t pipe_open(uint64_t id) {
    uint64_t idx;
    if ((idx = get_index(id)) == -1) {
        idx = create_pipe(id);
        if (idx == -1) {
            return -1;
        }
    }
    pipes[idx].total_processes++;
    return id;
}

uint64_t pipe_write(uint64_t id, char* str, uint64_t count) {
    uint64_t idx = get_index(id);
    if (idx == -1) {
        return -1;
    }
    uint64_t i = 0;

    t_pipe* pipe = &(pipes[idx]);

    while (i < count) {
        sem_wait(pipe->write_sem);

        pipe->buffer[pipe->step_to_write] = str[i];
        pipe->step_to_write = (pipe->step_to_write + 1) % BUFFER_SIZE;

        sem_post(pipe->read_sem);

        i++;
    }
    return id;
}

uint64_t pipe_read(uint64_t id) {
    uint64_t idx = get_index(id);
    if (idx == -1) {
        return -1;
    }

    t_pipe* pipe = &(pipes[idx]);

    sem_wait(pipe->read_sem);

    char c = pipe->buffer[pipe->step_to_read];
    pipe->step_to_read = (pipe->step_to_read + 1) % BUFFER_SIZE;

    sem_post(pipe->write_sem);

    return c;
}

uint64_t pipe_close(uint64_t id) {
    uint64_t idx = get_index(id);
    if (idx == -1) {
        return -1;
    }

    t_pipe* pipe = &(pipes[idx]);

    pipe->total_processes--;
    if (pipe->total_processes > 0) {
        return 1;
    }

    sem_close(pipe->read_sem);
    sem_close(pipe->write_sem);

    pipe->state = EMPTY;

    return 1;
}

void pipe_status() {
    printf("\nEstado de pipes activos\n");
    for (int i = 0; i < MAX_PIPES; i++) {
        t_pipe pipe = pipes[i];
        if (pipe.state == IN_USE) {
            printf("\tID: %d\n", pipe.id);
            printf("\tNro de procesos asociados: %d\n", pipe.total_processes);
            printf("\tSemaforo de read: %d\n", pipe.read_sem);
            printf("\tSemaforo de write: %d\n", pipe.write_sem);
            printf("\tPipe buffer content: ");
            for (int i = pipe.step_to_read; i != pipe.step_to_write; i = (i + 1) % BUFFER_SIZE) {
                putChar(pipe.buffer[i]);
            }
        }
    }
    printf("\n\n");
}

static uint64_t get_index(uint64_t id) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].state == IN_USE && pipes[i].id == id) {
            return i;
        }
    }
    return -1;
}

static uint64_t get_new_index() {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].state == EMPTY) {
            return i;
        }
    }
    return -1;
}

static uint64_t create_pipe(uint64_t id) {
    uint64_t idx;
    if ((idx = get_new_index()) == -1) {
        return -1;
    }

    t_pipe* pipe = &(pipes[idx]);

    pipe->id = id;
    pipe->state = IN_USE;
    pipe->step_to_read = pipe->step_to_write = pipe->total_processes = 0;

    if ((pipe->read_sem = sem_open(initial_sem++, 0)) == -1) {
        return -1;
    }
    if ((pipe->write_sem = sem_open(initial_sem++, BUFFER_SIZE)) == -1) {
        return -1;
    }

    return id;
}