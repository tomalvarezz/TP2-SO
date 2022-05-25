#ifndef _SYSCALLS_MANAGEMENT_H_
#define _SYSCALLS_MANAGEMENT_H_

#include <stdint.h>
#include <keyboard.h>
#include <time.h>
#include <memory_manager.h>

extern uint64_t RTC(uint64_t time);

uint64_t syscall_handler(uint64_t rdi, uint64_t rsi,uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax);
uint64_t sys_read_handler(uint64_t fd, char* buf, uint64_t count);
uint64_t sys_write_handler(uint64_t fd, const char* buf, uint64_t count);
uint64_t sys_time_handler(uint64_t time);
int sys_get_registers_handler(uint64_t* dest);
void write_registers(uint64_t* src);
void sys_get_memory_handler(uint8_t* start ,uint64_t* dump);
uint64_t sys_sleep_handler(uint64_t timeout_ms);

#endif