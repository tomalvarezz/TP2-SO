#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <scheduler_queue.h>

#define FOREGROUND 1
#define BACKGROUND 0

// scheduler
void initialize_scheduler();
int new_process(void (*entryPoint)(int, char**), int argc, char** argv,
                int foreground, int* fd);
extern void callTimerTick();
void* scheduler(void* sp);

// estado de proceso
int set_state(uint64_t pid, int new_state);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int ready_process(uint64_t pid);
int kill_current_FG_process();

// getters
int get_process_PID();
int get_current_process_read_FD();
int get_current_process_write_FD();

// priority
int set_priority(uint64_t pid, int new_priority);
int current_process_is_foreground();

// printers
void print_processes_status();
void print_current_process();

// sync
void yield();

#endif