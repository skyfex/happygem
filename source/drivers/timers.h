#ifndef TIMERS_H
#define TIMERS_H


#include "include/types.h"

extern uint8_t tick;
extern uint8_t tick192;

// Todo: (tick60 && tick%60==0) would be safer
// #define tick1 (tick180 && tick%180==0)
// #define tick4 (tick180 && tick%45==0)
// #define tick5 (tick180 && tick%36==0)
// #define tick10 (tick180 && tick%18==0)
// #define tick20 (tick180 && tick%9==0)
// #define tick30 (tick180 && tick%6==0)
#define tick64 (tick192 && tick%3==0)
#define tick32 (tick64 && tick%2==0)
#define tick16 (tick32 && tick%2==0)
#define tick8 (tick16 && tick%2==0)
#define tick4 (tick8 && tick%2==0)
#define tickexp (tick%16==0)


void tick_timer_init();
void tick_process();

#endif