#ifndef PIPES_H
#define PIPES_H

#include <stddef.h>
#include <stdint.h>


uint64_t pipe_open(uint64_t id);
uint64_t pipe_write(uint64_t id, char *str, uint64_t count);
uint64_t pipe_read(uint64_t id);
uint64_t pipe_close(uint64_t id);

//Función para mostrar estado de todos los pipes
void pipe_status();

#endif
