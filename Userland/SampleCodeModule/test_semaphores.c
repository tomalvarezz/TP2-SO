// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <test_util.h>
#include <syscalls.h>
#include <test_semaphores.h>

#define SEM_ID 50
#define TOTAL_PAIR_PROCESSES 2

int global; // shared memory

static void slowInc(uint64_t* p, uint64_t inc) {
    uint64_t aux = *p;
    sys_yield(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

static void inc(int argc, char* argv[]) {
    uint64_t sem = satoi(argv[1]);
    uint64_t value = satoi(argv[2]);
    uint64_t N = satoi(argv[3]);
    uint64_t i;

    if( sem ){
        printf("Comenzando testeo con semaforos\n");

        if (sys_sem_open(SEM_ID, 1) == -1) {
            printf("ERROR OPENING SEM\n");
            return;
        }

        for (i = 0; i < N; i++) {
            if (sys_sem_wait(SEM_ID) == -1) {
                printf("ERROR WAITING SEM\n");
                return;
            }
            slowInc(&global, value);
            if (sys_sem_post(SEM_ID) == -1) {
                printf("ERROR POSTING SEM\n");
                return;
            }
        }

        if (sys_sem_close(SEM_ID) == -1) {
            printf("ERROR CLOSING SEM\n");
            return;
        }
    }
    else{
        printf("Comenzando testeo sin semaforos\n");

        for (i = 0; i < N; i++) {
            slowInc(&global, value);
        }
    }   
}

void test_no_synchro() {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITHOUT SEM)\n");

    printf("Valor inicial: %d\n", global);

    //Primer proceso en background para que no sea bloqueante
    //Segundo proceso en foreground bloquea el wrapper
    //De esta forma se obliga a test_no_synchro a esperar que ambos procesos terminen antes del printf final
    char* argInc[] = {"Inc sin sem", "0", "1", "10000"};
    int pid1=sys_new_process(&inc, 4, argInc, BACKGROUND, 0);
    char* argDec[] = {"Inc sin sem", "0", "-1", "10000"};
    int pid2=sys_new_process(&inc, 4, argDec, FOREGROUND, 0);

    printf("Valor final: %d\n", global);
}

void test_synchro() {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITH SEM)\n");

    printf("Valor inicial: %d\n", global);

    //Primer proceso en background para que no sea bloqueante
    //Segundo proceso en foreground bloquea el wrapper
    //De esta forma se obliga a test_synchro a esperar que ambos procesos terminen antes del printf final
    char* argInc[] = {"Inc con sem", "1", "1", "10000"};
    int pid1=sys_new_process(&inc, 4, argInc, BACKGROUND, 0);
    char* argDec[] = {"Inc con sem", "1", "-1", "10000"};
    int pid2=sys_new_process(&inc, 4, argDec, FOREGROUND, 0);

    printf("Valor final: %d\n", global);
}