#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdint.h>

void exceptionDispatcher(uint64_t exception, uint64_t *stackFrame);
void backUpRipRsp(uint64_t *ip, uint64_t *rsp);

#endif 