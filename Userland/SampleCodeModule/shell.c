#include <shell.h>
#include <libc.h>
#include <commands.h>
#include <syscalls.h>

#define STDIN 0
#define STDOUT 1
#define USER 2
#define SIZE_BUF 100
#define MAX_ARGUMENTS 7
#define EOF -1
#define USERLAND_PID 1

static char user[SIZE_BUF] = {0};
static int userLen;
static int pipeId = 50;

static int getBuffer(char*, char**);
static void shellExecute();

static int find_pipe(int argc, char** argv);
static void pipe_routine(int pipe_pos, int argc, char** argv);
static int execute_pipe(int pipe_pos, int argc, char** argv);
static int pipe_command(int argc, char** argv, int fdin, int fdout, int foreground);

/*Funcion que se encarga de leer lo que ingrese el usuario constantemente hasta que escriba exit*/
void shell(int argc, char** argv) {
    printf("Ingrese nombre de usuario: ");
    userLen = sys_read(STDIN, user, SIZE_BUF);
    user[--userLen] = 0;

    printf("\nBienvenido!\nComando \'/help\' para mas ayuda\n\n");
    printf("Presione 'CTRL'+'r' en cualquier momento para guardar el valor de los registros\n");
    printf("Que modulo desea correr?\n");

    // sys_kill_process(USERLAND_PID);
    // sys_processes_status();
    shellExecute();
}

static void shellExecute() {
    int pipeIdx = -1;
    int buff_size;
    char buffer[SIZE_BUF] = {0};
    int foreground;

    while (1) {
        char* argv[MAX_ARGUMENTS] = {0};
        int argc;
        foreground = FOREGROUND;
        putChar('$');
        sys_write(USER, user, userLen);
        printf(": ");

        buff_size = sys_read(STDIN, buffer, SIZE_BUF);
        buffer[buff_size - 1] = 0;

        argc = getBuffer(buffer, argv);

        pipeIdx = find_pipe(argc, argv);

        /*if (argv[argc - 1][0] == '&') {
            foreground = BACKGROUND;
            argc--;
        }*/

        /*if (checkCommand(argv[0]) == -1) {
            printf("Comando invalido SHELL\n");
            error = 1;
        }*/

        if (pipeIdx >= 0) {
            pipe_routine(pipeIdx, argc, argv);
        } else {
            if (argv[argc - 1][0] == '&') {
                argc--;
                foreground = BACKGROUND;
            }
            runCommand(argc, argv, foreground, 0);
        }
    }
}

/*Lee de entrada estandar lo que escribe el usuario*/
static int getBuffer(char* buffer, char** argv) {
    int argc = 0;

    if (*buffer != ' ' && *buffer != '\0') {
        argv[argc++] = buffer;
    }

    while (*buffer != 0) {
        if (*buffer == ' ') {
            *buffer = 0;
            if ((*(buffer + 1) != ' ') && (*(buffer + 1) != 0)) {
                if (argc >= MAX_ARGUMENTS) {
                    return -1;
                }
                argv[argc++] = buffer + 1;
            }
        }
        buffer++;
    }
    return argc;
}

static int find_pipe(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        if (strCmp(argv[i], "|") == 0) {
            return i;
        }
    }
    return -1;
}

static void pipe_routine(int pipe_pos, int argc, char** argv) {
    if (pipe_pos == 0 || pipe_pos == argc - 1) {
        printf("\nPipe (|) debe ser usado entre dos comandos.\n\n");
        return;
    }
    int pipe = execute_pipe(pipe_pos, argc, argv);
    if (pipe == -1) {
        printf("\nUno de los comandos es invalido. Use /help.\n\n");
        return;
    }
}

static int execute_pipe(int pipe_pos, int argc, char** argv) {
    char* currentArgv[MAX_ARGUMENTS];
    int currentArgc = 0;
    int pids[2];

    int pipe = sys_pipe_open(pipeId++);
    if (pipe == -1) {
        printf("\nError creating pipe.\n");
        return -2;
    }

    for (int i = pipe_pos + 1, j = 0; i < argc; i++, j++) {
        currentArgv[j] = argv[i];
        currentArgc++;
    }

    //Proceso que lee del pipe
    pids[0] = pipe_command(currentArgc, currentArgv, BACKGROUND, pipe, STDOUT);
    if (pids[0] == -1) {
        sys_pipe_close(pipe);
        return -1;
    }

    currentArgc = 0;
    for (int i = 0; i < pipe_pos; i++) {
        currentArgv[i] = argv[i];
        currentArgc++;
    }

    //Proceso que escribe en el pipe
    pids[1] = pipe_command(currentArgc, currentArgv, FOREGROUND, STDIN, pipe);
    if (pids[1] == -1) {
        sys_pipe_close(pipe);
        return -1;
    }

    //int endOfFile = EOF;
    sys_pipe_write(pipe, (char*) EOF);

    sys_pipe_close(pipe);
    putChar('\n');
    return 1;
}

static int pipe_command(int argc, char** argv, int foreground, int fdin, int fdout) {
    int fd[2];

    if (checkCommand(argv[0]) == -1) {
        return -1;
    }

    fd[0] = fdin;
    fd[1] = fdout;

    return runCommand(argc, argv, foreground, fd);
}