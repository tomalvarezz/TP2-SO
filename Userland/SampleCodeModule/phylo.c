#include <libc.h>
#include <syscalls.h>

#define MAX_PHILOSOPHERS 10
#define INITIAL_PHILOSOPHERS_COUNT 6
#define SEM_MUTEX_ID 200
#define SEM_PHILOSOPHER_ID 300

#define LEFT(i) (((i) + philosopherCount - 1) % philosopherCount)
#define RIGHT(i) (((i) + 1) % philosopherCount)

#define DECIMAL_BASE 10

#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef struct t_philosofer {
    int pid;
    int sem;
    int ID;
    int state;
} t_philosopher;

t_philosopher* philosophers[MAX_PHILOSOPHERS];
static int philosopherCount = 0;
static int mutex;
static int stillEating = 1;

static int addPhilosopher();
static int removePhilosopher();
static void printTable(int argc, char** argv);
static void philosopherRoutine(int argc, char** argv);
static void takeForks(int id);
static void putForks(int id);
static void canEat(int id);

void phylosophersProblem(int argc, char** argv) {

    mutex = sys_sem_open(SEM_MUTEX_ID, 1);

    for (int i = 0; i < INITIAL_PHILOSOPHERS_COUNT; i++) {
        addPhilosopher();
    }

    printf("\nEsperando a 3 segundos que se sienten a comer los filosofos iniciales\n");

    sys_sleep(3000);

    char* args[] = {"Philosophers problem"};
    int tablePID = sys_new_process(&printTable, 1, args, BACKGROUND, 0);

    while (stillEating) {
        char key = getChar();
        if (key == 'f') {
            printf("\nFilosofos retirandose de la mesa\n");
            stillEating = 0;

        } else if (key == 'a') {
            if (addPhilosopher() == -1) {
                printf("\nNo se puede agregar mas de %d filosofos\n\n", MAX_PHILOSOPHERS);
            } else {
                printf("\nUn filosofo nuevo entro a la mesa\n\n");
            }
        }else if (key == 'r'){
            if (removePhilosopher() == -1) {
                printf("\nNo se puede remover al filosofo, minima cantidad: %d\n\n", INITIAL_PHILOSOPHERS_COUNT);
            } else {
                printf("\nEl filosofo se ha retirado de la mesa\n\n");
            }
        }
    }

    for (int i = 0; i < philosopherCount; i++) {
        sys_sem_close(philosophers[i]->sem);
        sys_kill_process(philosophers[i]->pid);
        free(philosophers[i]);
    }
    sys_kill_process(tablePID);
    sys_sem_close(mutex);
}

static int addPhilosopher() {
    if (philosopherCount == MAX_PHILOSOPHERS) {
        return -1;
    }

    sys_sem_wait(mutex);

    t_philosopher* philosopher = malloc(sizeof(t_philosopher));

    philosopher->sem = sys_sem_open(SEM_PHILOSOPHER_ID + philosopherCount, 1);
    philosopher->state = THINKING;
    philosopher->ID = philosopherCount;

    char philosopherIDStr[3];
    itoa(philosopherCount, philosopherIDStr, DECIMAL_BASE);

    char* argv[] = {"philosopher", philosopherIDStr};
    philosopher->pid = sys_new_process(&philosopherRoutine, 2, argv, BACKGROUND, 0);

    philosophers[philosopherCount++] = philosopher;

    sys_sem_post(mutex);

    return 0;
}

static int removePhilosopher() {
  if (philosopherCount == INITIAL_PHILOSOPHERS_COUNT) {
    return -1;
  }

  sys_sem_wait(mutex);

  t_philosopher *philosopher = philosophers[--philosopherCount];
  sys_sem_close(philosopher->sem);
  sys_kill_process(philosopher->pid);
  free(philosopher);

  sys_sem_post(mutex);
  return 0;
}

static void printTable(int argc, char** argv) {
    while (stillEating) {
        sys_sem_wait(mutex);
        int i;
        for (i = 0; i < philosopherCount; i++) {
            if (philosophers[i]->state == EATING) {
                putChar('E');
            } else {
                putChar('.');
            }
            putChar(' ');
        }
        putChar('\n');
        sys_sem_post(mutex);
        sys_yield();
    }
}

static void philosopherRoutine(int argc, char** argv) {
    int id = satoi(argv[1]);
    while (1) {
        takeForks(id);
        // tiempo que tarda en pensar o comer
        sys_sleep(1000);
        putForks(id);
        // tiempo que tarda en pensar o comer
        sys_sleep(1000);
    }
}

static void takeForks(int id) {
    sys_sem_wait(mutex);
    philosophers[id]->state = HUNGRY;
    canEat(id);
    sys_sem_post(mutex);
    sys_sem_wait(philosophers[id]->sem);
}

static void putForks(int id) {
    sys_sem_wait(mutex);
    philosophers[id]->state = THINKING;
    canEat(LEFT(id));
    canEat(RIGHT(id));
    sys_sem_post(mutex);
}

static void canEat(int id) {
  if (philosophers[id]->state == HUNGRY && philosophers[LEFT(id)]->state != EATING && philosophers[RIGHT(id)]->state != EATING) {
    philosophers[id]->state = EATING;
    sys_sem_post(philosophers[id]->sem);
  }
}