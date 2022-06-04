// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler_queue.h>

void queue_process(t_process_list *processes, t_process_node *process){
  if (queue_is_empty(processes)) {
    processes->first = process;
    processes->last = processes->first;

  } else {  
    processes->last->next = process;
    process->next = NULL;
    processes->last = process;
  
  }

  if (process->pcb.state == READY) {
    processes->ready_size++;
  }

  processes->size++;
}

t_process_node *dequeue_process(t_process_list *processes){
  if (queue_is_empty(processes)) {
    return NULL;
  }

  t_process_node *to_return = processes->first;
  processes->first = processes->first->next;
  processes->size--;

  if (to_return->pcb.state == READY) {
    processes->ready_size--;
  }

  return to_return;
}

int queue_is_empty(t_process_list *processes){
    return processes->size==0;
}