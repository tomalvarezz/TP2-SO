#ifndef _IRQDISPATCHERSOFTWARE_H_
#define _IRQDISPATCHERSOFTWARE_H_

#include <stdint.h>

void irqDispatcherSoftware(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax, uint64_t irq, uint64_t rsp);
void int_80(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rsp);

#endif