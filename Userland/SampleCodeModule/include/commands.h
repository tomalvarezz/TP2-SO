#ifndef _COMMANDS_H_
#define _COMMANDS_H_

int runCommand(int argc, char* argv[], int foreground, int* fd);
int checkCommand(char* com);

#endif