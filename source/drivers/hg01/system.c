#include "common.h"
#include "drivers/system.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void system_init()
{
   // Make sure JTAG is enabled when the system starts
   system_enable_jtag();
}

void system_enable_int()
{
   sei();
}

void system_disable_int()
{
   cli();
}

void system_disable_jtag()
{
   // We must have a delay before disabling jtag
   // otherwise we will have no way to program
   // the device
   _delay_ms(2000);

   MCUCR_struct.jtd = 1;
   MCUCR_struct.jtd = 1;
}

void system_enable_jtag()
{
   MCUCR_struct.jtd = 0;
   MCUCR_struct.jtd = 0; 
}