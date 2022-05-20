#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <scheduler_queue.h>

#define FOREGROUND 1
#define BACKGROUND 0

//scheduler
void initialize_scheduler();
int new_process(void (*entryPoint)(int, char **), int argc, char **argv,
               int foreground, int *fd);
void callTimerTick();
void *scheduler(void *sp);

//estado de proceso
int set_state(uint64_t pid, int new_state);
int kill_process(uint64_t pid);
int block_process(uint64_t pid);
int ready_process(uint64_t pid);
//Hecho hasta acá

void kill_current_FG_process();

//getters
int get_process_PID();
int get_current_process_input_FD();
int get_current_process_output_FD();

//priority
void set_priority(uint64_t pid, int new_priority);
int current_process_is_foreground();

//printers
void print_process_status();
void print_current_process();

//sync
void wait(int pid);
void yield();

#endif