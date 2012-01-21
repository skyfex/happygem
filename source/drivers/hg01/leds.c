
/* ==== LED Driver ====
 * Author: Audun Wilhelmsen
 *
 * This driver can drive the LEDs on the happygem at various colors by PWM'ing them
 * (alternating between turning them on and off). It updates the LEDs through the
 * SPI module. It uses interrupts to continually update the LEDs.
 *
 * TODO: Simple mode 
 *
 * LED Pins:
 * --------------------
 * PB0: SSn (Chip select / Latch output - I/O)
 * PB1: SCK (Clock)
 * PB2: MOSI (Data)
 * PB5: OEn (Output enable - I/O, Active Low)
 *
 */
   
#include "common.h"
#include "drivers/leds.h"
#include <avr/interrupt.h>

#define BUF_SIZE 16

struct {
   uint8_t on;
   uint8_t mode;
   uint8_t brightness;
   
   uint8_t buffers[BUF_SIZE][6];
   uint8_t idx1;
   uint8_t idx2;
   uint8_t idx1_cnt;
} leds;


ISR(SPI_STC_vect)
{
	if (!leds.on) return;
   uint8_t idx1 = leds.idx1;
   uint8_t idx2 = leds.idx2;
   if (idx2==0) {
      setPin(B, P0);
      clrPin(B, P0);
   }
   leds.idx2++;
   if (leds.idx2==6) {
      leds.idx2 = 0;
      if (leds.idx1_cnt==leds.idx1*2) {
         leds.idx1++;
         leds.idx1_cnt = 0;
      }
      else
         leds.idx1_cnt++;
      if (leds.idx1==BUF_SIZE) {
         leds.idx1=0;
      }
   }
   SPDR = leds.buffers[idx1][idx2];
}


void leds_set(uint8_t led_idx, uint8_t r, uint8_t g, uint8_t b)
{

   
   uint8_t r_idx=0, r_mask=0, g_idx=0, g_mask=0, b_idx=0, b_mask = 0;
   
   #define LED(IX, RX, RM, GX, GM, BX, BM) \
      case IX:                            \
      r_idx = RX; r_mask = 1<<RM;           \
      g_idx = GX; g_mask = 1<<GM;           \
      b_idx = BX; b_mask = 1<<BM;           \
      break;                                
   
   switch(led_idx) {
      LED( 0,   3,1,   3,0,   2,7)      
      LED( 1,   2,6,   2,5,   2,4)
      LED( 2,   2,3,   2,2,   2,1)
      LED( 3,   2,0,   1,7,   1,6)
      LED( 4,   1,5,   1,4,   1,3)
      LED( 5,   1,2,   1,1,   1,0)
      LED( 6,   0,7,   0,6,   0,5)
      LED( 7,   0,4,   0,3,   0,2)
      LED( 8,   5,7,   0,1,   0,0)
      LED( 9,   5,4,   5,5,   5,6)
      LED(10,   5,1,   5,2,   5,3)
      LED(11,   4,6,   4,7,   5,0)
      LED(12,   4,5,   4,4,   4,3)
      LED(13,   4,2,   4,1,   4,0)
      LED(14,   3,5,   3,6,   3,7)
      LED(15,   3,2,   3,3,   3,4)  
   }      
   
   int i;
   for (i=0;i<r;i++)
      leds.buffers[i][r_idx] |= r_mask;
   for (i=r;i<16;i++)
      leds.buffers[i][r_idx] &= ~r_mask;
      
   for (i=0;i<g;i++)
      leds.buffers[i][g_idx] |= g_mask;
   for (i=g;i<16;i++)
      leds.buffers[i][g_idx] &= ~g_mask;
      
   for (i=0;i<b;i++)
      leds.buffers[i][b_idx] |= b_mask;
   for (i=b;i<16;i++)
      leds.buffers[i][b_idx] &= ~b_mask;
}

void leds_set_all(led_t framebuffer[16])
{
   int i;
   for (i=0;i<16;i++)
      leds_set(i, framebuffer[i].r, framebuffer[i].g, framebuffer[i].b);
}

void leds_clear()
{
   int i,j;
   for (i=0;i<BUF_SIZE;i++) {
      for (j=0;j<6;j++) 
         leds.buffers[i][j] = 0x00;
   }  
}

void leds_on(void)
{
   leds.on = 1;
   leds.idx1 = 0;
   leds.idx1_cnt = 0;   
   leds.idx2 = 0;
   leds.idx2++;
   SPDR = leds.buffers[0][0];
   clrPin(B, P5);
}
void leds_off(void)
{
   leds.on = 0;
   setPin(B, P5);
}


void leds_init(void)
{


   //Enable Output Pins
   enablePinOutput(B, P5|P2|P1|P0);
   
	// Enable SPI Interrupt, Enable SPI, Master Mode, Clock=fosc/4
	SPCR = (1<<SPIE)|(1<<SPE)|(1<<MSTR);
   SPCR_struct.spr = 0;

   // LED Output enable
   clrPin(B, P5);
   // Make sure Latch signal is low 
   clrPin(B, P0);
	
	// Clear buffer
   leds_clear();
   
   leds.on = 1;
   leds.idx1 = 0;
   leds.idx1_cnt = 0;   
   leds.idx2 = 0;
   leds.idx2++;
   SPDR = leds.buffers[0][0];

}
