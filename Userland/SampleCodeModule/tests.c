#include <tests.h>
#include <libc.h>
#include <test_util.h>
#include <syscalls.h>

#define MAX_BLOCKS 128
#define MAX_MEMORY (32 * 1024 * 1024)

#define MAX_PROCESSES 10


typedef struct MM_rq {
    void* address;
    uint32_t size;
} mm_rq;

void test_mm() {
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory = MAX_MEMORY;

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = malloc(mm_rqs[rq].size);

            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    printf("test_mm ERROR\n");
                    return;
                }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                free(mm_rqs[i].address);
    }
}

typedef struct P_rq{
  int pid;
  int state;
}p_rq;

void test_processes(){
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  char * argvAux[] = {"Proceso Test"};

  p_rq p_rqs[MAX_PROCESSES];

  while (1){
    // Create max_processes processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      p_rqs[rq].pid = sys_new_process(&endless_loop, 1, argvAux, BACKGROUND, 0);

      if (p_rqs[rq].pid == -1){
        printf("test_processes: ERROR creating process\n");
        return;
      }else{
        p_rqs[rq].state = READY;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){
      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(100) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == READY || p_rqs[rq].state == BLOCKED){
              if (sys_kill_process(p_rqs[rq].pid) == -1){  
                printf("test_processes: ERROR killing process\n");
                return;
              }
              p_rqs[rq].state = FINISHED; 
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == READY){
              if(sys_block_process(p_rqs[rq].pid) == -1){
                printf("test_processes: ERROR blocking process\n");
                return;
              }
              p_rqs[rq].state = BLOCKED; 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2){
          if(sys_ready_process(p_rqs[rq].pid) == -1){
            printf("test_processes: ERROR unblocking process\n");
            return;
          }
          p_rqs[rq].state = READY; 
        }
    } 
  }
}