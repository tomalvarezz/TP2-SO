#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

#define FOREGROUND 1
#define BACKGROUND 0

#define READY 0
#define BLOCKED 1
#define FINISHED 2

extern uint64_t sys_read(uint64_t fd, char* buff, uint64_t count);
extern uint64_t sys_write(uint64_t fd, const char* buff, uint64_t count);
extern uint64_t sys_time(uint64_t time);
extern uint64_t sys_clear();
extern uint64_t sys_get_registers(uint64_t* registers);
extern uint64_t sys_get_memory(uint64_t* address, uint64_t* dump);

//Syscalls de memory
extern uint64_t sys_malloc(uint64_t malloc_bytes);
extern uint64_t sys_free(void* block);
extern uint64_t sys_memory_dump(void);
extern uint64_t sys_sleep(uint64_t tenth_of_seconds);

//Syscalls scheduling
extern uint64_t sys_new_process(void (*entryPoint)(int, char**), int argc, char** argv, int foreground, int* fd);
extern uint64_t sys_kill_process(uint64_t pid);
extern uint64_t sys_block_process(uint64_t pid);
extern uint64_t sys_ready_process(uint64_t pid);
extern uint64_t sys_get_process_pid();
extern uint64_t sys_set_priority(uint64_t pid, int new_priority);
extern uint64_t sys_processes_status();
extern uint64_t sys_current_process_status();
extern uint64_t sys_get_process_state(uint64_t pid);
extern uint64_t sys_yield();

//Syscalls semaphores
extern uint64_t sys_sem_open(uint32_t id, uint64_t initial_value);
extern uint64_t sys_sem_wait(uint32_t id);
extern uint64_t sys_sem_post(uint32_t id);
extern uint64_t sys_sem_close(uint32_t id);
extern uint64_t sys_sem_status();

//Syscalls pipes
extern uint64_t sys_pipe_open(uint32_t id, uint64_t initial_value);
extern uint64_t sys_pipe_write(uint32_t id);
extern uint64_t sys_pipe_read(uint32_t id);
extern uint64_t sys_pipe_close(uint32_t id);
extern uint64_t sys_pipe_status();

extern uint64_t sys_invalid_op_code();

#endif