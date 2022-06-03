#include <test_mm.h>
#include <libc.h>
#include <test_util.h>
#include <syscalls.h>

#define MAX_BLOCKS 128
#define MAX_MEMORY (32 * 1024 * 1024)


typedef struct MM_rq {
    void* address;
    uint32_t size;
} mm_rq;

void test_mm() {
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory = MAX_MEMORY;

    int pid = sys_get_process_pid();
    printf("\nTesteo con PID: %d\n", pid);

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