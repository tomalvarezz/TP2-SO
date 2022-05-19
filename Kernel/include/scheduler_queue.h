#ifndef SCHEDULER_QUEUE_H
#define SCHEDULER_QUEUE_H

#include <stdint.h>
#include <stddef.h>

#define READY 0
#define BLOCKED 1
#define FINISHED 2

typedef struct {
  //Info
  int pid;
  int ppid;
  int is_foreground;
  int state;
  uint64_t priority;
  int fd[2];

  char name[30];

  //stackframes
  void *RSP;
  void *RBP;

  //arguments
  int argc;
  char **argv;
} PCB;
//Process Control Block

typedef struct t_process_node {
  PCB pcb;
  struct t_process_node *next;
} t_process_node;

typedef struct t_process_list {
  uint32_t size;
  uint32_t ready_size;
  t_process_node *first;
  t_process_node *last;
} t_process_list;

void queue_process(t_process_list *processes, t_process_node *process);
t_process_node *dequeue_process(t_process_list *processes);
int queue_is_empty(t_process_list *processes);

#endif