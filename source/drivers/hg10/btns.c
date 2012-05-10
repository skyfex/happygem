
/* ==== Buttons Driver ====
 * Author: Audun Wilhelmsen
 * 
 *
 */
 
#include "common.h"
#include "drivers/btns.h"
#include <avr/interrupt.h>

struct {
   btns_callback_t callback;
   uint8_t flag;
   
   } btns;

ISR(INT6_vect)
{
   // Btn 1
}
ISR(INT7_vect)
{
   // Btn 2
}
ISR(INT0_vect)
{
   // Btn 3
}
ISR(PCINT0_vect)
{
   // Btn 4
}

void btns_init(btns_callback_t callback)
{
   btns.callback = callback;
   btns.flag = 0x00;
   
   // Btn 1 and 2
   enablePinInput(E, P6|P7);
   enablePinPullup(E, P6|P7);

   // EICRB_struct.isc6 = 0x2;
   // EICRB_struct.isc7 = 0x2;
   // EIMSK |= (1<<6)|(1<<7);

   // Btn 3
   enablePinInput(D, P0);
   enablePinPullup(D, P0);

   // EICRA_struct.isc0 = 0x2;
   // EIMSK |= 1;

   // Btn 4
   enablePinInput(B, P4);
   enablePinPullup(B, P4);

   // PCICR |= (1<<0);
   // PCMSK0 |= (1<<4);

}

void btns_process()
{
    
   // Btn 1
   if (!getPin(E, P6))  
      btns.flag |=  (1<<1);
   else                 {  if (btns.flag & (1<<1))  btns.callback(1); btns.flag &= ~(1<<1); }
   // Btn 2                  
   if (!getPin(E, P7))  
      btns.flag |=  (1<<2);
   else                 {  if (btns.flag & (1<<2))  btns.callback(2); btns.flag &= ~(1<<2); }
   // Btn 3                  
   if (!getPin(D, P0))     
      btns.flag |=  (1<<3);
   else                 {  if (btns.flag & (1<<3))  btns.callback(3); btns.flag &= ~(1<<3); }
   // Btn 4                  
   if (!getPin(B, P4))  
      btns.flag |=  (1<<4);
   else                 {  if (btns.flag & (1<<4))  btns.callback(4); btns.flag &= ~(1<<4); }
}