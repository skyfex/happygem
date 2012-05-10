
#ifndef DRIVERS_ALL_H
#define DRIVERS_ALL_H

#include "drivers/system.h"
#include "drivers/btns.h"
#include "drivers/usart.h"
#include "drivers/rf.h"
#include "drivers/eeprom.h"
#include "drivers/timers.h"
#include "drivers/sound.h"

#ifdef DRIVERS_hg01
#include "drivers/leds01.h"
#else
#include "drivers/leds.h"
#endif

//void drivers_init_all()
//{
   //// system_init();
   //// usart_init();
   //// btns_init();
   //// leds_init();
   //// rf_init();
//}

#endif