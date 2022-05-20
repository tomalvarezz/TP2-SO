#include <stdint.h>
#include <scheduler.h>
#include <libraryc.h>
#include <interrupts.h>

static void *const sampleCodeModuleAddress = (void *)0x400000;

static t_process_node *current_process = NULL;
static uint64_t current_PID = 0;
static uint64_t cycles_left;
static t_process_list *processes;
static t_process_node *base_process;

#define SIZE_OF_STACK (4 * 1024)
#define BACKGROUND_PRIORITY_DEFAULT 1
#define FOREGROUND_PRIORITY_DEFAULT 2
#define MAX_PRIORITY 50

typedef struct {
  uint64_t gs;
  uint64_t fs;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rbp;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;

  uint64_t rip;
  uint64_t cs;
  uint64_t eflags;
  uint64_t rsp;
  uint64_t ss;
  uint64_t base;
} t_stack_frame;

static uint64_t get_PID();
static void idle_process(int argc, char **argv);
static void initialize_process_stack_frame(void (*entryPoint)(int, char **), int argc, char **argv, void *rbp);
static int initialize_pcb(PCB *PCB, char *name, uint8_t foreground, int *fd);
static int get_arguments(char **to, char **from, int count);
static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv);
static void free_process(t_process_node *process);
static t_process_node *get_process(uint64_t pid);
static void end();


void initialize_scheduler() {
  processes = malloc(sizeof(t_process_list));

  //Preguntar si hay que hacer un handler para error
  //Si falla el initialize scheduler se rompe todo
  if (processes == NULL) {
    return;
  }
  
  processes->first = NULL;
  processes->last = processes->first;
  processes->ready_size = 0;
  processes->size = 0;

  char *argv[] = {"Proceso IDLE inicial"};
  new_process(&idle_process, 1, argv, BACKGROUND, 0);
  base_process = dequeue_process(processes);
}

void *scheduler(void *sp){
  if (current_process != NULL) {
    if (current_process->pcb.state == READY && cycles_left > 0) {
      cycles_left--;
      return sp;
    }

    current_process->pcb.RSP = sp;

    if (current_process->pcb.pid != base_process->pcb.pid) {
      if (current_process->pcb.state == FINISHED) {
        /*llamo proceso padre si lo tuviera para desbloquearlo, aun sin implementar*/
        free_process(current_process);
      } else {
        queue_process(processes, current_process);
      }
    }
  }

  if (processes->ready_size > 0) {
    current_process = dequeue_process(processes);
    
    while (current_process->pcb.state != READY) {
      //No habría que realizar los mismos chequeos que más arriba 
      //para ver si tiene que desbloquear o no un proceso padre?
      if (current_process->pcb.state == FINISHED) {
        free_process(current_process);
      }
      if (current_process->pcb.state == BLOCKED) {
        queue_process(processes, current_process);
      }
      current_process = dequeue_process(processes);
    }
  } else {
    current_process = base_process;
  }

  cycles_left = current_process->pcb.priority;

  return current_process->pcb.RSP;
}

int new_process(void (*entry_point)(int, char **), int argc, char **argv,
               int foreground, int *fd) {
  
  if (entry_point == NULL) {
    return -1;
  }
 
  t_process_node *new_process = malloc(sizeof(t_process_node));
  if (new_process == NULL) {
    return -1;
  }

  if (initialize_pcb(&new_process->pcb, argv[0], foreground, fd) == -1) {
    free(new_process);
    return -1;
  }
  
  char **arguments = malloc(sizeof(char *) * argc);
  if (arguments == NULL) {
    free(new_process);
    return -1;
  }

  if (get_arguments(arguments, argv, argc) == -1) {
    free(new_process);
    free(arguments);
    return -1;
  }

  new_process->pcb.argc = argc;
  new_process->pcb.argv = arguments;

  initialize_process_stack_frame(entry_point, argc, arguments, new_process->pcb.RBP);

  new_process->pcb.state = READY;

  queue_process(processes, new_process);

  //Bloquear al proceso padre si estamos en FG y tiene proceso padre
  //Aun no implementado
  /*if (new_process->pcb.is_foreground && new_process->pcb.ppid) {
    block_process(new_process->pcb.ppid);
  }*/

  return new_process->pcb.pid;
}

static void idle_process(int argc, char **argv) {
 while (1) {
   sys_sleep_handler(2000);
   printf("soy el proceso idle \n");
  }
}

static uint64_t get_PID() { return current_PID++; }

static int initialize_pcb(PCB *PCB, char *name,uint8_t foreground, int *fd){
  strCpy(PCB->name, name);
  
  PCB->pid = get_PID();

  PCB->ppid = (current_process == NULL ? 0 : current_process->pcb.pid);

  if (foreground > 1) {
    return -1;
  }

  PCB->is_foreground = (current_process == NULL ? foreground : (current_process->pcb.is_foreground ? foreground : 0));

  PCB->RBP = malloc(SIZE_OF_STACK);

  if (PCB->RBP == NULL) {
    return -1;
  }

  PCB->priority = PCB->is_foreground ? FOREGROUND_PRIORITY_DEFAULT
                                  : BACKGROUND_PRIORITY_DEFAULT;

  PCB->fd[0] = (fd ? fd[0] : 0);
  PCB->fd[1] = (fd ? fd[1] : 1);

  PCB->RBP = (void *)((char *)PCB->RBP + SIZE_OF_STACK - 1);

  PCB->RSP = (void *)((t_stack_frame *)PCB->RBP - 1);
  return 0;
}

static int get_arguments(char **to, char **from, int count) {
  for (int i = 0; i < count; i++) {
    to[i] = malloc(sizeof(char) * (strLen(from[i]) + 1));

    if (to[i] == NULL) {
      i--;
      while (i >= 0) {
        free(to[i]);
        i--;
      }
      return -1;
    }
    strCpy(to[i], from[i]);
  }
  return 0;
}

static void end() {
  kill_process(current_process->pcb.pid);
  callTimerTick();
}

static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv) {
  entryPoint(argc, argv);
  end();
}

static void initialize_process_stack_frame(void (*entryPoint)(int, char **),
                                        int argc, char **argv, void *rbp) {
  t_stack_frame *stackFrame = (t_stack_frame *)rbp - 1;

  stackFrame->gs = 0x001;
  stackFrame->fs = 0x002;
  stackFrame->r15 = 0x003;
  stackFrame->r14 = 0x004;
  stackFrame->r13 = 0x005;
  stackFrame->r12 = 0x006;
  stackFrame->r11 = 0x007;
  stackFrame->r10 = 0x008;
  stackFrame->r9 = 0x009;
  stackFrame->r8 = 0x00A;
  stackFrame->rsi = (uint64_t)argc;
  stackFrame->rdi = (uint64_t)entryPoint;
  stackFrame->rbp = 0x00B;
  stackFrame->rdx = (uint64_t)argv;
  stackFrame->rcx = 0x00C;
  stackFrame->rbx = 0x00D;
  stackFrame->rax = 0x00E;
  stackFrame->rip = (uint64_t)wrapper;
  stackFrame->cs = 0x008;
  stackFrame->eflags = 0x202;
  stackFrame->rsp = (uint64_t)(&stackFrame->base);
  stackFrame->ss = 0x000;
  stackFrame->base = 0x000;
}

static void free_process(t_process_node *process) {
  for (int i = 0; i < process->pcb.argc; i++) {
    free(process->pcb.argv[i]);
  }
  free(process->pcb.argv);
  free((void *)((char *)process->pcb.RBP - SIZE_OF_STACK + 1));
  free((void *)process);
}

//new_state puede ser READY, BLOCKED O FINISHED
int set_state(uint64_t pid, int new_state){
   t_process_node *process = get_process(pid);

  if (process == NULL || process->pcb.state == FINISHED) {
    return -1;
  }

  if (process == current_process) {
    process->pcb.state = new_state;
    return process->pcb.pid;
  }

  //Chequeos para ver si hay que incrementar o no ready_size
  if (process->pcb.state != READY && new_state == READY) {
    processes->ready_size++;
  }
  if (process->pcb.state == READY && new_state != READY) {
    processes->ready_size--;
  }

  process->pcb.state = new_state;

  return process->pcb.pid;
}

static t_process_node *get_process(uint64_t pid) {
  //Si el proceso es el current lo retorno directamente
  if (current_process != NULL && current_process->pcb.pid == pid) {
    return current_process;
  }

  t_process_node *process = processes->first;
  while (process != NULL) {
    if (process->pcb.pid == pid) {
      return process;
    }
    process = (t_process_node *)process->next;
  }
  return NULL;
}

int kill_process(uint64_t pid){
  int PID_to_return=set_state(pid, FINISHED);

  if(current_process!=NULL &&  pid==current_process->pcb.pid){
    callTimerTick();
  }

  return PID_to_return;
}
int block_process(uint64_t pid){
  int PID_to_return=set_state(pid, BLOCKED);

  if(current_process!=NULL &&  pid==current_process->pcb.pid){
    callTimerTick();
  }

  return PID_to_return;
}
int ready_process(uint64_t pid){
  return set_state(pid, READY);
}

int kill_current_FG_process(){
     
  if (current_process != NULL && current_process->pcb.is_foreground &&
      current_process->pcb.state == READY)
  {
   return kill_process(current_process->pcb.pid);
  }

  return -1;

}

int get_process_PID(){

  if(current_process!=NULL){
  return current_process->pcb.pid;
  }
  return -1;
}

int get_current_process_read_FD(){
  if (current_process != NULL)
  {
    return current_process->pcb.fd[0];
  }
  return -1;
}

int get_current_process_write_FD(){
  if (current_process != NULL)
  {
    return current_process->pcb.fd[1];
  }
  return -1;
}

void set_priority(uint64_t pid, int new_priority){
  if (new_priority < 0)
  {
    new_priority = 0;
  }
  if (new_priority > MAX_PRIORITY)
  {
    new_priority = MAX_PRIORITY;
  }

  t_process_node *process = get_process(pid);

  if (process != NULL)
  {
    process->pcb.priority = new_priority;
  }

}

int current_process_is_foreground(){
  if(current_process!=NULL){ 
  return current_process->pcb.is_foreground;
}
 }

 char* print_process_status(int state){
   switch (state)
   {
   case READY:
     return "READY";
     break;
   case BLOCKED:
     return "BLOCKED";
   default:
     return "FINISHED";
     break;
   };
 }
 
 void print_current_process(){

   if(current_process==NULL){
     return;
   }

       printf("Name: %s\n",current_process->pcb.name);
      
       printf("PID: %d\n",current_process->pcb.pid);

       printf("PPID: %d\n", current_process->pcb.ppid);

       printf("%s\n", current_process->pcb.is_foreground > 0 ? "FOREGROUND" : "BACKGROUND");

       printf("RSP: %x\n",current_process->pcb.RSP);

       printf("RBP: %x\n", current_process->pcb.RBP);

       printf("Priority: %d\n",current_process->pcb.priority);

       printf("State: %s\n\n",print_process_status(current_process->pcb.state));
 }

 void wait(int pid){
   //if(current_process!=NULL){
     //current_process->pcb.state=BLOCKED;
   //}

   t_process_node *process = getProcess(pid);
   if (process)
   {
     process->pcb.is_foreground = 1;
     block_process(current_process->pcb.pid);
   }
 }

 void yield(){
   cycles_left = 0;
   callTimerTick();
 }