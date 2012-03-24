#ifndef TIMERS_H
#define TIMERS_H


#include "include/types.h"

extern uint8_t tick;
extern uint8_t tick60;

// Todo: (tick60 && tick%60==0) would be safer
#define tick1 (tick%60==0)

void tick_timer_init();
void tick_process();

#endif