#include <shell.h>
#include <libc.h>
#include <commands.h>
#include <syscalls.h>

#define STDIN 0
#define USER 2
#define SIZE_BUF 100
#define MAX_ARGUMENTS 7
#define USERLAND_PID 1

static char user[SIZE_BUF] = {0};
static int userLen;

static int getBuffer(char*, char**);
static void shellExecute();
/*static int find_pipe(int argc, char **argv);
static void pipe_routine(int pipe_pos, int argc, char **argv);
static int execute_pipe(int pipe_pos, int argc, char **argv);
static int pipe_command(int argc, char **argv, int fdin, int fdout, int foreground);*/

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
    // int pipePos;
    int buff_size;
    char buffer[SIZE_BUF] = {0};
    char error = 0;
    int foreground;

    while (1) {
        char* argv[MAX_ARGUMENTS] = {0};
        int argc;
        foreground=FOREGROUND;
        putChar('$');
        sys_write(USER, user, userLen);
        printf(": ");

        buff_size = sys_read(STDIN, buffer, SIZE_BUF);
        buffer[buff_size - 1] = 0;

        argc = getBuffer(buffer, argv);

        if (argv[argc - 1][0] == '&') {
          foreground = BACKGROUND;
          argc--;
        }

        if (checkCommand(argv[0]) == -1) {
            printf("Comando invalido SHELL\n");
            error = 1;
        }

        if (!error) {
            printf("Corriendo el comando: %s\n", argv[0]);
            runCommand(argc, argv, foreground, 0);
        }

        // pipePos=find_pipe(argc, argv);

        // if(pipePos >= 0){
        //     pipe_routine(pipePos, argc, argv);
        // }else{
        //     if(*argv[argc-1] == '&'){
        //         argc--;
        //         sys_new_process(&runCommand, argc, argv, BACKGROUND, 0);

        //     }else{
        //         sys_new_process(&runCommand, argc, argv, FOREGROUND, 0);
        //     }
        // }

        error = 0;
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

/*
static int find_pipe(int argc, char **argv){
  for (int i = 0; i < argc; i++) {
    if (strCmp(argv[i], "|") == 0) {
      return i;
    }
  }
  return -1;
}

static void pipe_routine(int pipe_pos, int argc, char **argv){
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

static int execute_pipe(int pipe_pos, int argc, char **argv){

}

static int pipe_command(int argc, char **argv, int fdin, int fdout, int foreground){

}*/