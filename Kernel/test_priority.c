#include <stdint.h>
#include <test_priority.h>
#include <scheduler.h>
#include <libraryc.h>
#include <test_util.h>

#define TOTAL_PROCESSES 10

void test_priority() {
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    char *argv[] = {"Idle Process"};
    pids[i] = new_process(&idleProcess, 1, argv, BACKGROUND, NULL);
  }

  busyWait(TOTAL_PROCESSES * MAJOR_WAIT);

  printf(
      "\nCHANGING PRIORITIES...\n\nCHANGING PRIORITIES...\n\nCHANGING "
      "PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    switch (i % 3) {
      case 0:
        set_priority(pids[i], 1);  // lowest priority
        break;
      case 1:
        set_priority(pids[i], 25);  // medium priority
        break;
      case 2:
        set_priority(pids[i], 50);  // highest priority
        break;
    }
  }

  busyWait(TOTAL_PROCESSES * MAJOR_WAIT);

  printf("\nBLOCKING...\n\nBLOCKING...\n\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    block_process(pids[i]);
  }

  printf(
      "\nCHANGING PRIORITIES WHILE BLOCKED...\n\nCHANGING PRIORITIES WHILE "
      "BLOCKED...\n\nCHANGING PRIORITIES WHILE BLOCKED...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++) {
    set_priority(pids[i], 25);
  }

  printf("\nUNBLOCKING...\n\nUNBLOCKING...\n\nUNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    ready_process(pids[i]);
  }

  busyWait(TOTAL_PROCESSES * MAJOR_WAIT);
  printf("\nKILLING...\n\nKILLING...\n\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    kill_process(pids[i]);
  }
}
