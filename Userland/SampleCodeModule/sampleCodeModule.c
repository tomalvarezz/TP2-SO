/* sampleCodeModule.c */
#include <shell.h>
#include <syscalls.h>

int main() {
    char *argv_shell[] = {"Shell"};
    sys_new_process(&shell, 1, argv_shell, FOREGROUND, 0);
    return 0;
}