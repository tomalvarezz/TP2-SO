#ifndef _TEST_UTIL_H_
#define _TEST_UTIL_H_

#include <stdint.h>

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT      10000000 // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print();

#endif