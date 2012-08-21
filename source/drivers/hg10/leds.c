
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
 * PB1: SCK (Clock - SPI)
 * PB2: MOSI (Data to chip - SPI)
 * PB3: MISO (Data from chip - SPI)
 * PG5: PWMCLK (PWM Clock signal - TIMER0)
 *
 * PB5: Blue Source
 * PB6: Red Source
 * PB7: Green Source
 */

 #define REDPIN P6
 #define BLUPIN P5
 #define GRNPIN P7
   
#include "common.h"
#include "drivers/leds.h"
#include <avr/interrupt.h>


#define F_CPU 16000000
#include <util/delay.h>

#define BUF_SIZE 15

struct {
   uint8_t on;
   uint8_t mode;
   uint8_t max_bright;
   uint8_t brightness;
   uint16_t driver_conf;
   
   uint16_t buffers[3][16];
   uint8_t idx;
   uint8_t color_idx;
} leds;


ISR(SPI_STC_vect)
{

}


void leds_set(uint8_t led_idx, uint16_t r, uint16_t g, uint16_t b)
{

}

void leds_set_all(led_t framebuffer[16])
{
   int i;
   for (i=0;i<16;i++) {
      leds.buffers[0][i] = framebuffer[i].r;
      leds.buffers[1][i] = framebuffer[i].g;
      leds.buffers[2][i] = framebuffer[i].b;
   }
}

void leds_clear()
{
   int i;
   for (i=0;i<16;i++) {
      leds.buffers[0][i] = 0;
      leds.buffers[1][i] = 0;
      leds.buffers[2][i] = 0;
   }  
}

void leds_on(void)
{
}
void leds_off(void)
{

}

void leds_spi_write(uint16_t data)
{
   SPCR_struct.spe = 1;  // SPI enable
   SPDR = (data>>8)&0xFF; // Write upper half of data
   while (!SPSR_struct.spif) {} // Wait to finish
   SPDR = (data)&0xFF; // Write lower half of data
   while (!SPSR_struct.spif) {}
   SPCR_struct.spe = 0;
}

uint8_t leds_spi_read()
{
   uint8_t data;
   SPCR_struct.spe = 1;  // SPI enable
   SPDR = 0x00; // Initiate transfer
   while (!SPSR_struct.spif) {} // Wait to finish
   data = SPDR; // Read data
   SPCR_struct.spe = 0;  
   return data;
}


void leds_data_latch(uint16_t data)
{
   for (uint8_t i=0; i<16; i++) {
      // Output bit
      if (data&0x8000)
         setPin(B, P2);
      else
         clrPin(B, P2);

      // Data latch requires 1 clock cycle LE
      if (i==15) setPin(B, P0); 
      // Strobe clock
      setPin(B, P1);
      clrPin(B, P1);
      // Shift data
      data = data<<1;
   }
   clrPin(B, P0);
}

void leds_global_latch(uint16_t data)
{

   for (uint8_t i=0; i<16; i++) {
      // Output bit
      if (data&0x8000)
         setPin(B, P2);
      else
         clrPin(B, P2);
      // Global latch requires 2 clock cycle LE
      if (i>=14) setPin(B, P0);
      // Strobe clock
      setPin(B, P1);
      clrPin(B, P1);
      // Shift data
      data = data<<1;
   }
   clrPin(B, P0);
}

void leds_write_conf(uint16_t data)
{
   for (uint8_t i=0; i<16; i++) {
      // Output bit
      if (data&0x8000)
         setPin(B, P2);
      else
         clrPin(B, P2);
      // Write conf requires 10 clock cycle LE
      if (i>=6) setPin(B, P0);
      // Strobe clock
      setPin(B, P1);

      // uint8_t x = getPin(B, P3);
      // print_uchar(x);

      clrPin(B, P1);


      // Shift data
      data = data<<1;
   }
   clrPin(B, P0);


   // for (uint8_t i=0; i<16; i++) {
   //    setPin(B, P1);

   //    uint8_t x = getPin(B, P3);
   //    print_uchar(x);

   //    clrPin(B, P1);

   // }
}

uint8_t leds_read_conf()
{
   // enablePinInput(B, P3);
   // enablePinPullup(B, P3);

   // uint8_t x;
   // for (uint8_t i=0; i<32; i++) {

   //    setPin(B, P2);
 
   //    setPin(B, P1);

   //    x = getPin(B, P3);
   //    print_uchar(x);

   //    clrPin(B, P1);

   //    x = getPin(B, P3);
   //    print_uchar(x);

   // }

   // uint8_t data;
   // SPCR_struct.spe = 1;  // SPI enable
   // SPDR = 0x55; // Initiate transfer
   // while (!SPSR_struct.spif) {} // Wait to finish
   // data = SPDR; // Read data
   // print_uchar(data);
   // SPDR = 0x55; // Initiate transfer
   // while (!SPSR_struct.spif) {} // Wait to finish
   // data = SPDR; // Read data
   // print_uchar(data);
   // SPCR_struct.spe = 0;  


   // clrPin(B, P0);

   for (uint8_t i=0; i<19; i++) {
      if (i>=12) setPin(B, P0);
      // Strobe clock
      setPin(B, P1);
      clrPin(B, P1);
   }
   clrPin(B, P0);  

   // for (uint8_t i=0; i<16; i++) {
   //    // if (i>=11) setPin(B, P0);
   //    // Strobe clock
   //    uint8_t x = getPin(B, P3);
   //    print_uchar(x);

   //    setPin(B, P1);
   //    clrPin(B, P1);
   // }
   // print("\n");

   // uint8_t data;
   // data = leds_spi_read();
   // print_uchar(data); print("\n");
   // data = leds_spi_read();
   // print_uchar(data); print("\n");

   // return data;
}

void leds_init(void)
{
   // Config for LED driver (from datasheet)
   // F -- 0: 16-bit transfer; 1: 256-bit
   // A -- 0: Automatic PWM sync; 1: Manual
   // 9..2 -- Current gain (brightness)

   //                   FEDCBA9876543210
   leds.driver_conf = 0b1001111000000000;
   leds.idx = 0;
   leds.color_idx = 0;
   leds.brightness = 128;
   leds.max_bright = 255;

   // Make sure pins are set to low
   clrPin(B, P2);
   clrPin(B, P1);
   clrPin(B, P0);

   // Color power off
   setPin(B, REDPIN);
   setPin(B, GRNPIN);
   setPin(B, BLUPIN);

   //Enable Output Pins
   enablePinOutput(B, P7|P6|P5 | P2|P1|P0);

   enablePinInput(B, P3);
   disablePinPullup(B, P3);

   leds_write_conf(leds.driver_conf);

   // Enable SPI, Master Mode, Clock=fosc/4
   // Don't Enable interrupt
   SPCR = (1<<MSTR); 
   SPCR_struct.spr = 0;
   SPSR_struct.spi2x = 1;


	// Clear buffer
   leds_clear();

   // Setup TIMER0 for PWMCLK
   TCCR0A_struct.com0b = 1; // Toggle on compare match
   TCCR0A_struct.wgm0 = 2;
   TCCR0B_struct.wgm02 = 0;
   OCR0B = 0;
   OCR0A = 8;
   TCCR0B_struct.cs0 = 0x01; // Timer clock = CPU clock

   enablePinOutput(G, P5); // Enable PG5 / OC0B output

   // leds_read_conf();
}

void leds_set_brightness(uint8_t bright)
{
   if (bright >= 16) bright = 15;

   uint8_t bright_tbl[16] = {
      0, 1, 2, 4, 8, 16, 32, 64,
      128, 129, 130, 132, 136, 144, 160, 255
   };

   leds.brightness = bright;
   leds.driver_conf = (leds.driver_conf & 0b1111110000000011) | (bright_tbl[bright] << 2);
   leds_write_conf(leds.driver_conf); 
}

uint8_t leds_get_brightness()
{
   return leds.brightness;
}

void leds_process()
{

   leds_read_conf();
   while (1) {}

   // Color power off
   setPin(B, REDPIN);
   setPin(B, GRNPIN);
   setPin(B, BLUPIN);

   uint8_t i;
   for (i=0;i<15;i++)
      leds_spi_write(leds.buffers[leds.color_idx][15-i]);

   leds_global_latch(leds.buffers[leds.color_idx][0]);

   if (leds.color_idx==0) clrPin(B, REDPIN);
   else if (leds.color_idx==1) clrPin(B, GRNPIN);
   else if (leds.color_idx==2) clrPin(B, BLUPIN);

   leds.color_idx = (leds.color_idx+1)%3;

   // if (leds.idx==0) {
   //    // Color power off
   //    setPin(B, REDPIN);
   //    setPin(B, GRNPIN);
   //    setPin(B, BLUPIN);
   // }
   // if (leds.idx<15) {
   //    leds_spi_write(leds.buffers[leds.color_idx][leds.idx]);
   //    // leds_data_latch(leds.buffers[leds.color_idx][leds.idx]);
   //    if (leds.idx==0) {
   //       if (leds.color_idx==0) clrPin(B, REDPIN);
   //       else if (leds.color_idx==1) clrPin(B, GRNPIN);
   //       else if (leds.color_idx==2) clrPin(B, BLUPIN);
   //    }
   //    leds.idx++;
   // }
   // else {
   //    leds_global_latch(leds.buffers[leds.color_idx][leds.idx]);
   //    leds.color_idx = (leds.color_idx+1)%3;
   //    leds.idx = 0;
   //    _delay_ms(1);
   // }
}

