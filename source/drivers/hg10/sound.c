
#include "common.h"
#include <drivers/usart.h>
#include <drivers/sound.h>
#include <avr/interrupt.h>

struct {
   uint8_t dur_i;
   uint8_t sound_i;
   uint8_t delay_i;
   uint8_t repeat_i;
   sound_pattern_t *current_pattern; 
   } snd;



ISR(TIMER4_COMPA_vect)
{
   PIND = (1<<3);
}

void sound_stop()
{
   snd.dur_i = 0;
   snd.sound_i = 0;
   snd.repeat_i = 0;
   snd.delay_i = 0;
   snd.current_pattern = 0;
   
   TCCR4B_struct.cs4 = 0;  
}

void sound_init()
{
   DDRD |= (1<<3); // Enable output on sound pin
   
   TCCR4A_struct.wgm4 = 0;
   TCCR4B_struct.wgm4 = 1;
   OCR4A = 3822;
   TIMSK4_struct.ocie4a = 1;
   TCCR4B_struct.cs4 = 0;  

   sound_stop();
}

static void sound_update()
{
   uint16_t val = snd.current_pattern->sounds[snd.sound_i].val;
   if (val==0 || snd.delay_i) {
      TCCR4B_struct.cs4 = 0;
   }
   else {
      OCR4A = snd.current_pattern->sounds[snd.sound_i].val;
      TCNT4 = 0;
      TCCR4B_struct.cs4 = 2;
   }
}

void sound_play(sound_pattern_t *pattern)
{
   sound_stop();
   snd.current_pattern = pattern;
   sound_update();
}

void sound_process()
{
   if (snd.current_pattern) {
      if (snd.delay_i) {
         snd.delay_i--;
         if (!snd.delay_i)
            sound_update();
      }
      else {
         snd.dur_i++;
         if (snd.dur_i == snd.current_pattern->sounds[snd.sound_i].dur) {
            snd.dur_i = 0;
            snd.sound_i++;
            if (snd.current_pattern->delay)
               snd.delay_i = snd.current_pattern->delay;
            if (snd.sound_i == snd.current_pattern->count) {
               snd.sound_i = 0;
               snd.repeat_i++;
               if (snd.repeat_i == snd.current_pattern->repeat) {
                  sound_stop();
                  return;
               }
            }
            sound_update();  
         }
      }
   }  
}