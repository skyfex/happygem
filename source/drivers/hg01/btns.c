
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

void btns_init(btns_callback_t callback)
{
   btns.callback = callback;
   btns.flag = 0x00;
   
   // Btn 3 and 4
   enablePinInput(G, P2|P3);
   enablePinPullup(G, P2|P3);
   
   // Btn 1 and 2
   enablePinInput(F, P6|P4);
   enablePinPullup(F, P6|P4);
}

void btns_process()
{
   // if (!(btns.flag & (1<<1)) & !getPin(F, P4)) {
   //    btns.callback(1);
   //    btns.flag &= ~(1<<1);
   // }
   // if (!(btns.flag & (1<<2)) & !getPin(F, P6))
   //    btns.callback(2);
   //    btns.flag &= ~(1<<1);
   // }
   // if (!(btns.flag & (1<<3)) & !getPin(G, P2))
   //    btns.callback(3);
   //    btns.flag &= ~(1<<1);
   // }
   // if (!(btns.flag & (1<<4)) & !getPin(G, P3))
   //    btns.callback(4);
   //    btns.flag &= ~(1<<1);
   // }
      
   // Btn 1
   if (!getPin(F, P4))  btns.flag |=  (1<<1);
   else                 {  if (btns.flag & (1<<1))  btns.callback(1); btns.flag &= ~(1<<1); }
   // Btn 2                  
   if (!getPin(F, P6))  btns.flag |=  (1<<2);
   else                 {  if (btns.flag & (1<<2))  btns.callback(2); btns.flag &= ~(1<<2); }
   // Btn 3                  
   if (!getPin(G, P2))     btns.flag |=  (1<<3);
   else                 {  if (btns.flag & (1<<3))  btns.callback(3); btns.flag &= ~(1<<3); }
   // Btn 4                  
   if (!getPin(G, P3))  btns.flag |=  (1<<4);
   else                 {  if (btns.flag & (1<<4))  btns.callback(4); btns.flag &= ~(1<<4); }
}