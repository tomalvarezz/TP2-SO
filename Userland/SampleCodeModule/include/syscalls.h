#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

extern uint64_t sys_read(uint64_t fd,char* buff, uint64_t count);
extern uint64_t sys_write(uint64_t fd, const char* buff, uint64_t count);
extern uint64_t sys_time(uint64_t time);
extern uint64_t sys_clear();
extern uint64_t sys_get_registers(uint64_t* registers);
extern uint64_t sys_get_memory(uint64_t* address, uint64_t* dump);
extern uint64_t sys_malloc(uint64_t malloc_bytes);
extern uint64_t sys_free(void* block);
extern uint64_t sys_memory_dump(void);
extern uint64_t sys_sleep(uint64_t tenth_of_seconds);
extern uint64_t sys_invalid_op_code();

#endif