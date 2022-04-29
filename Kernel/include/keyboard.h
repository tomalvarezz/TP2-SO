#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

void keyboard_handler();
char bring_key();
char * getBuffer();
void cleanBuffer();
int sizeBuffer();
char kbHit();

#endif