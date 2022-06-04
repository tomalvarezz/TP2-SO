// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <test_util.h>
#include <syscalls.h>
#include <test_semaphores.h>

#define SEM_ID 50
#define TOTAL_PAIR_PROCESSES 2

uint64_t global; // shared memory

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

    for (int j = 0; j < argc; j++)
    {
        printf("argv[%d]: %s\n", j, argv[j]);
    }
    

    if (sem && sys_sem_open(SEM_ID, 1) == -1) {
        printf("ERROR OPENING SEM\n");
        return;
    }

    for (i = 0; i < N; i++) {
        if (sem && sys_sem_wait(SEM_ID) == -1) {
            printf("ERROR WAITING SEM\n");
        }
        slowInc(&global, value);
        if (sem && sys_sem_post(SEM_ID) == -1) {
            printf("ERROR POSTING SEM\n");
        }
    }

    if (sem && sys_sem_close(SEM_ID) == -1) {
        printf("ERROR CLOSING SEM\n");
    }
}

void test_no_synchro() {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITHOUT SEM)\n");

    printf("Valor inicial: %d\n", global);

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* argInc[] = {"Inc sin sem", "0", "1", "100000"};
        sys_new_process(&inc, 4, argInc, BACKGROUND, 0);
        char* argDec[] = {"Inc sin sem", "0", "-1", "100000"};
        sys_new_process(&inc, 4, argDec, BACKGROUND, 0);
    }

    printf("Valor final: %d\n", global);
}

void test_synchro() {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITH SEM)\n");

    printf("Valor inicial: %d\n", global);

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        char* argInc[] = {"Inc con sem", "1", "1", "100000"};
        sys_new_process(&inc, 4, argInc, BACKGROUND, 0);
        char* argDec[] = {"Inc con sem", "1", "-1", "100000"};
        sys_new_process(&inc, 4, argDec, BACKGROUND, 0);
    }

    printf("Valor final: %d\n", global);
}