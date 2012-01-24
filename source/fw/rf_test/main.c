#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define F_CPU 16000000
#include <util/delay.h>

enum States { S_SNOOP, S_SPAM };
uint8_t state = S_SNOOP;
uint16_t spam_delay = 0;
#define GEM_ADDR 9999
#define PAN_ADDR 1337

#define BUF_SIZE 16
typedef struct {
	char filled;
	rf_packet_t packet;
	uint8_t data;
} packet_buffer_t;
packet_buffer_t buffers[16];


void btn_handler(uint8_t btn_id)
{
   if(btn_id==3){
      print("Snoop mode\n");
	  state = S_SNOOP;
	  spam_delay = 0;
   }
   if(btn_id==4){
      print("Spam mode\n");
		state = S_SPAM;
		spam_delay++;
		if (spam_delay>4)
			spam_delay = 0;
		print("Spam delay: "); print_ushort(spam_delay); putc('\n');			
   }
}

void rf_rx_handler(rf_packet_t *packet)
{
	if (state==S_SNOOP) {
		uint8_t data = packet->data[0];
		for (uint8_t i=0;i<BUF_SIZE;i++) {
			if (!buffers[i].filled) {
				memcpy(&buffers[i].packet, packet, sizeof(rf_packet_t));
				buffers[i].data = data;
				buffers[i].filled = 1;
				return;
			}				
		}
		print("Buffer overflow\n");
	}
}

void fw_main()
{
	for (int i=0;i<BUF_SIZE;i++)
		buffers[i].filled = 0;
	
   // Init drivers
   system_init();
   usart_init();
   btns_init(btn_handler);                                                                                 
   rf_init(PAN_ADDR, GEM_ADDR, rf_rx_handler);

   system_enable_int();

   print("RF Test Gem\n");

   while(1) {
	if (state==S_SPAM) {
		rf_tx(GEM_ADDR+1, 'x', 0);	
		switch(spam_delay) {
			case 0:
			break;
			case 1:
			_delay_us(1);
			break;
			case 2:
			_delay_us(16);
			break;
			case 3:
			_delay_us(64);
			break;
			case 4:
			_delay_us(256);
			break;
		}
		
	}		
	if (state==S_SNOOP) {
		for (uint8_t i=0; i<BUF_SIZE; i++) {
			if (buffers[i].filled) {
				rf_packet_t *packet = &buffers[i].packet;
				if (packet->has_source)
					print(" S:"); print_ushort(packet->source_addr);
				if (packet->has_dest)
					print(" D:"); print_ushort(packet->dest_addr);
				print(" L:"); print_uchar(packet->length);
				print(" T:"); putc(buffers[i].data);
				print(" ED:"); print_uchar(packet->ed);
				putc('\n');
				buffers[i].filled = 0;
			}
		}	
	}		
    btns_process();
   }
}