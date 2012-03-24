#include <avr/io.h>
#include "drivers/hg01/common.h"
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000
#include <util/delay.h>
#include <avr/sleep.h>

uint8_t gem_id;


bool rf_rx_handler(rf_packet_t *packet)
{
 
   return true;
}

void led_write(uint16_t *buf)
{
   // P5 = PWCLK
   // P2 = MOSI
   // P1 = CLK
   // P0 = LA
   uint8_t i,j;
   for (i=0; i<16; i++) {
      uint16_t val = buf[i];
      for (uint8_t j=0; j<16; j++) {
         if (val&0x8000)
            setPin(B, P2);
         else
            clrPin(B, P2);
         if (i==15 && j==14) setPin(B, P0);
         if (j==15) setPin(B, P0);
         setPin(B, P1);
         clrPin(B, P1);
         val = val<<1;
      }
      clrPin(B, P0);
      print("\n");
   }
}

void fw_main()
{

   // Init drivers
   system_init();
   usart_init();
   
   gem_id = eeprom_read(0x20);
   
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");
   print("Test\n");

   // rf_init(1337, gem_id, rf_rx_handler);
 
   // leds_init();   
   
   // Init services
   // anim_init();

   // sound_init();

   //Enable Output Pins
   // P5 = PWCLK
   // P2 = MOSI
   // P1 = CLK
   // P0 = LA
   enablePinOutput(B, P5|P2|P1|P0);
   
   // Enable SPI, Master Mode, Clock=fosc/4, Don't Enable interrupt
   // SPCR = (1<<SPE)|(1<<MSTR);//|(1<<SPIE); 
   // SPCR_struct.spr = 0;
   
   // SPSR_struct.spi2x = 1;

   // LED Output enable
   // clrPin(B, P5);
   // Make sure Latch signal is low 
   // clrPin(B, P5);
   clrPin(B, P2);
   clrPin(B, P1);
   clrPin(B, P0);
   
   uint16_t buffer[16] = {
      0, 0, 0, 0,
      0, 1000, 1000, 0,
      0, 0, 0, 0,
      0, 0, 0, 0
   };
   uint16_t buffer2[16] = {
      0, 0, 0, 0,
      0, 0, 0, 1000,
      0, 0, 0, 0,
      0, 0, 0, 0
   };
   // led_write(buffer);

   TCCR1A_struct.com1a = 1;
   TCCR1A_struct.wgm1 = 0;
   TCCR1B_struct.wgm1 = 1;
   OCR1A = 8;
   TCCR1B_struct.cs1 = 0x01; // Timer clock = CPU clock
   TIMSK1_struct.ocie1a = 1;

   enablePinOutput(G, P3);
   enablePinOutput(G, P2);
   setPin(G, P3);
   setPin(G, P2);

   uint32_t i,j;	
   while(1) {
      // _delay_ms(100);
      // buffer[0] += 100;
      // led_write(buffer);
      // for (i=0;i<4096*500;i++) {
      // setPin(B, P5);
      // clrPin(B, P5);
      // }
      // SMCR_struct.sm = 0;
      // SMCR_struct.se = 1;
      // sleep_cpu();
      _delay_ms(1);
      setPin(G, P2);
      led_write(buffer);
      clrPin(G, P3);

      _delay_ms(1);
      setPin(G, P3);
      led_write(buffer2);
      clrPin(G, P2);
      // buffer[0]++;
      // led_write(buffer);
   }
}