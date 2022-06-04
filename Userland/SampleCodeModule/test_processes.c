// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <test_util.h>
#include <syscalls.h>
#include <test_processes.h>

#define MAX_PROCESSES 10

typedef struct P_rq {
    int pid;
    int state;
} p_rq;

void test_processes() {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    char* argvAux[] = {"Proceso Test"};

    p_rq p_rqs[MAX_PROCESSES];

    int pid = sys_get_process_pid();
    printf("\nTesteo con PID: %d\n", pid);

    while (1) {
        // Create max_processes processes
        for (rq = 0; rq < MAX_PROCESSES; rq++) {
            p_rqs[rq].pid = sys_new_process(&endless_loop, 1, argvAux, BACKGROUND, 0);

            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process\n");
                return;
            } else {
                p_rqs[rq].state = READY;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {
            for (rq = 0; rq < MAX_PROCESSES; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == READY || p_rqs[rq].state == BLOCKED) {
                            if (sys_kill_process(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR killing process\n");
                                return;
                            }
                            p_rqs[rq].state = FINISHED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == READY) {
                            if (sys_block_process(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR blocking process\n");
                                return;
                            }
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < MAX_PROCESSES; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
                    if (sys_ready_process(p_rqs[rq].pid) == -1) {
                        printf("test_processes: ERROR unblocking process\n");
                        return;
                    }
                    p_rqs[rq].state = READY;
                }
        }
    }
}