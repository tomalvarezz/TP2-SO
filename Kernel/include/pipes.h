#ifndef PIPES_H
#define PIPES_H

#include <stddef.h>
#include <stdint.h>


int pipe_open(int id);
int pipe_write(int id, char *str);
int pipe_read(int id);
int pipe_close(int id);

//Función para mostrar estado de todos los pipes
void pipe_status();

#endif
