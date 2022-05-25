#include <libraryc.h>
#include "test_util.h"
#include <scheduler.h>
#include <test_processes.h>

#define MINOR_WAIT 1000000
#define MAJOR_WAIT 10000000

enum State { RUNNING,
             BLOCK,
             KILLED };

typedef struct P_rq {
    int32_t pid;
    enum State state;
} p_rq;

uint64_t test_processes() {
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes = 15;
    char* argv[] = {"Idle Process"};
    uint64_t finish = 0;

    p_rq p_rqs[max_processes];

    while (finish++ < 500) {
        printf("Finish: %d\n", finish);
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++) {
            p_rqs[rq].pid = new_process(&idleProcess, 1, argv, BACKGROUND, NULL);

            if (p_rqs[rq].pid == -1) {
                printf("test_processes: ERROR creating process\n");
                return -1;
            } else {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {

            for (rq = 0; rq < max_processes; rq++) {
                action = GetUniform(100) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCK) {
                            if (kill_process(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR killing process\n");
                                return -1;
                            }
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING) {
                            if (block_process(p_rqs[rq].pid) == -1) {
                                printf("test_processes: ERROR blocking process\n");
                                return -1;
                            }
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
                    if (ready_process(p_rqs[rq].pid) == -1) {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
    }
    printf("Chupate esta agodio!!!\n");
    return 1;
}
