#include <stdint.h>
#include <time.h>

static unsigned long ticks = 0;
extern int _hlt();
int ellapsed = 0; // Used in sleep(), cleared by sleep(), incremented by timer_handler()

void timer_handler() {
	ticks++;
	ellapsed += 55;  //timer ticks every 55ms (taught in class)
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void sleep(int millis){
	ellapsed = 0;
	while (ellapsed<millis)
	{
		_hlt();
	}
}

int ms_elapsed() {
    return ticks*5000/91;
}

void timer_wait(int delta) { 
	int initialTicks = ticks;
	while(ticks - initialTicks < delta) {
		_hlt();
	}
}