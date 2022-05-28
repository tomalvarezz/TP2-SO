#include <time.h>

static unsigned long ticks = 0;
static unsigned long start_time = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

int millis(){
	return ticks*5000/91;
}
