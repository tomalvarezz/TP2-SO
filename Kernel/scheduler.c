#include <stdint.h>
#include <scheduler.h>
#include <scheduler_queue.h>
#include <interrupts.h>
#include <memory_manager.h>
#include <libraryc.h>

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
} t_stackFrame;

static void idle_process(int argc, char **argv);
static void initialize_process_stack_frame(void (*entryPoint)(int, char **), int argc, char **argv, void *rbp);
static int initialize_pcb(PCB *PCB, char *name, uint8_t foreground, int *fd);
static int get_arguments(char **to, char **from, int count);
static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv);


void initialize_process_manager() {
  
  processes = malloc(sizeof(t_process_list));
  if (processes == NULL) {
    return;
  }
  
  
  processes->first = NULL;
  processes->last = processes->first;
  processes->ready_size = 0;
  processes->size = 0;

  char *argv[] = {"Initial Idle Process"};
  new_process(&idle_process, 1, argv, BACKGROUND, 0);
  
  base_process = dequeue_process(processes);
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

  if (new_process->pcb.is_foreground && new_process->pcb.ppid) {
    blockProcess(new_process->pcb.ppid);
  }

  return new_process->pcb.pid;
}

static void idle_process(int argc, char **argv) {
  while (1) {
    printf("soy el proceso idle \n");
    _hlt();
  }
}

static int initialize_pcb(PCB *PCB, char *name,uint8_t foreground, int *fd){

  strcpy(PCB->name, name);
  
  PCB->pid = getPID();

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

  PCB->RSP = (void *)((t_stackFrame *)PCB->RBP - 1);
  return 0;
}

static int get_arguments(char **to, char **from, int count) {
  for (int i = 0; i < count; i++) {
    to[i] = malloc(sizeof(char) * (strlen(from[i]) + 1));

    if (to[i] == NULL) {
      i--;
      while (i >= 0) {
        free(to[i]);
        i--;
      }
      return -1;
    }
    strcpy(to[i], from[i]);
  }
  return 0;
}

static void initialize_process_stack_frame(void (*entryPoint)(int, char **),
                                        int argc, char **argv, void *rbp) {
  t_stackFrame *stackFrame = (t_stackFrame *)rbp - 1;

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


static void end() {
  (void)killProcess(current_process->pcb.pid);
  _callTimerTick();
}

static void wrapper(void (*entryPoint)(int, char **), int argc, char **argv) {
  entryPoint(argc, argv);
  end();
}
