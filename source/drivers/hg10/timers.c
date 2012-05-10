
#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/timers.h"


uint8_t tick;
uint8_t tick180;
uint8_t tick_prev;

ISR(TIMER1_COMPA_vect)
{   
	tick++;
	if (tick==240) tick = 0;
}

void tick_timer_init()
{
   TCCR1A_struct.wgm1 = 0;
   TCCR1B_struct.wgm1 = 1;
   OCR1A = 86; // 180Hz
   TCCR1B_struct.cs1 = 0x05; // Timer clock = CPU clock / 1024
   TIMSK1_struct.ocie1a = 1;
}

void tick_process()
{
	if (tick!=tick_prev) {
		tick180=1;
		tick_prev = tick;
	}
	else
		tick180 = 0;
}