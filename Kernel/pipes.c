#include <pipes.h>
#include <semaphores.h>

#define BUFFER_SIZE 1204
#define MAX_PIPES 8

#define IN_USE 1
#define EMPTY 0


static int initial_sem = 1000;

typedef struct t_pipe {
    int id;
    char buffer[BUFFER_SIZE];
    int step_to_write, step_to_read;
    long total_processes;
    int write_sem, read_sem;
    int state;
} t_pipe;

static t_pipe pipes[MAX_PIPES];

static int create_pipe(int id);
static int get_new_index();
static int get_index(int id);
static int pipe_writer(char c, int idx);

int pipe_open(int id) {
    int idx;
    if ((idx = get_index(id)) == -1) {
        idx = create_pipe(id);
        if (idx == -1) {
            return -1;
        }
    }
    pipes[idx].total_processes++;
    return id;
}

int pipe_write(int id, char* str) {
    int idx = get_index(id);
    if (idx == -1) {
        return -1;
    }
    int i = 0;

    while (str[i] != 0) {
        pipe_writer(str[i], idx);
        i++;
    }

    return id;
}

int pipe_read(int id) {
    int idx = get_index(id);
    if (idx == -1) {
        return -1;
    }

    t_pipe* pipe = &(pipes[idx]);

    sem_wait(pipe->read_sem);

    char c = pipe->buffer[pipe->step_to_read];
    pipe->step_to_read = (pipe->step_to_read + 1) % BUFFER_SIZE;

    sem_post(pipe->step_to_write);

    return c;
}

int pipe_close(int id){
    int idx = get_index(id);
  if (idx == -1) {
    return -1;
  }

  t_pipe *pipe = &(pipes[idx]);

  pipe->total_processes--;
  if (pipe->total_processes > 0) {
    return 1;
  }

  sem_close(pipe->read_sem);
  sem_close(pipe->write_sem);

  pipe->state = EMPTY;

  return 1;
}

static int get_index(int id) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].state == IN_USE && pipes[i].id == id) {
            return i;
        }
    }
    return -1;
}

static int get_new_index() {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].state == EMPTY) {
            return i;
        }
    }
    return -1;
}

static int create_pipe(int id) {
    int idx;
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

static int pipe_writer(char c, int idx) {
    t_pipe* pipe = &(pipes[idx]);

    sem_wait(pipe->write_sem);

    pipe->buffer[pipe->step_to_write] = c;
    pipe->step_to_write = (pipe->step_to_write + 1) % BUFFER_SIZE;

    sem_post(pipe->read_sem);

    return 0;
}