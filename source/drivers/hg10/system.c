#include "common.h"
#include "drivers/system.h"
#include "drivers/eeprom.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <stdlib.h>

uint16_t system_boot_cnt;
uint16_t system_id;

void system_init()
{
   system_enable_power();
   // Make sure JTAG is enabled when the system starts
   system_enable_jtag();

   // system_boot_cnt = ((uint16_t)eeprom_read(0x24))<<8 + (uint16_t)eeprom_read(0x25);
   // system_boot_cnt++;
   // eeprom_write(0x24, system_boot_cnt>>8);
   // eeprom_write(0x25, system_boot_cnt&0xFF);
}

void system_enable_power()
{
   // Make sure it's pin is open
   enablePinInput(D, P6);
}
void system_disable_power()
{
   enablePinOutput(D, P6);
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

void system_srand()
{
   uint16_t seed = battery_measure();
   // seed *= system_boot_cnt;
   srand(seed);
}

void system_sleep()
{
   set_sleep_mode(2); // Power down
   sleep_enable();
   sleep_cpu();
   sleep_disable();
}

uint16_t battery_measure()
{
   DDRF &= ~(1<<1); // Configure pin PF1/ADC1 for input

   ADCSRA_struct.adate = 0; // No auto trigger
   ADCSRA_struct.adps = 0b111; // Clock division: 128
   ADMUX_struct.refs = 0b01; // 1.8V reference
   ADMUX_struct.adlar = 0; // Right adjust    
   ADMUX_struct.mux = 0b00001; // Single ended Channel 1

   ADCSRA_struct.aden = 1; // Enable ADC

   ADCSRA_struct.adsc = 1; // Start conversion


   uint16_t result;

   while (ADCSRA_struct.adsc!=0) {}

   result = ADC;

   ADCSRA_struct.aden = 0;

   return result;
}
