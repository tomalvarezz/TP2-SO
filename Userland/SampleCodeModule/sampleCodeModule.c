// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include <shell.h>
#include <syscalls.h>

int main() {
    char *argv_shell[] = {"Shell"};
    sys_new_process(&shell, 1, argv_shell, FOREGROUND, 0);
    return 0;
}