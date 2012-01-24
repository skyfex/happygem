#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
#include <stdlib.h>
// #include <util/delay.h>

enum States { HUG_SEARCH, HUG_REQ, GOT_HUG, GAVE_HUG };

#define GEM_ADDR 0
#define HUG_RANGE 130

typedef struct {
	uint8_t source;
	uint8_t type;
	uint8_t data;
} tiny_packet_t;

tiny_packet_t tp_buf[16];
uint8_t tp_ind = 0;
bool got_packet = 0;

tiny_packet_t* tp_handle(char type)
{
	got_packet = 0;
	for (uint8_t i=0; i<16; i++) {
		if (tp_buf[i].type==type) {
			tp_buf[i].type = 0; // Mark as handled
			return &tp_buf[i];
		}
	}
	return NULL;
}

void tp_clear()
{
	for (uint8_t i=0; i<16; i++) 
		tp_buf[i].type = 0;
}

uint8_t peer_table[16][3];

uint16_t tick = 0;

ISR(TIMER1_COMPA_vect)
{   
	tick++;
}

bool find_hug(uint8_t *addr_out)
{
	for(uint8_t i=0; i<16; i++) {
		if ((peer_table[i][0]+peer_table[i][1])>HUG_RANGE) {
			*addr_out = i;
			return 1;
		}
	}
	return 0;
}

void btn_handler(uint8_t btn_id)
{
   if(btn_id==3){
      print("Btn 3\n");

   }
   if(btn_id==4){
      print("Btn 4\n");

   }
}

void rf_rx_handler(rf_packet_t *packet)
{
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
   uint8_t addr = packet->source_addr;
   if (type=='p') {
	   //peer_table[addr][2] = peer_table[addr][1];
	   peer_table[addr][1] = peer_table[addr][0];
	   peer_table[addr][0] = packet->ed;
   }
   else {
	   //if (tp_buf[tp_ind].type) dbg_print("Buffer overflow");
	   tp_buf[tp_ind].source = addr;
	   tp_buf[tp_ind].type = type;
	   tp_buf[tp_ind].data = data;
	   tp_ind++;
	   got_packet = 1;
   }
   //putc(type); print(" "); print_uchar(data); print("\n");
}

void fw_main()
{
   // Init drivers
   system_init();
   usart_init();
   btns_init(btn_handler);                                                                                 
   rf_init(1337, GEM_ADDR, rf_rx_handler);
 
   leds_init();   
   
   // Init services
   anim_init();
   
   TCCR1A_struct.wgm1 = 0;
   TCCR1B_struct.wgm1 = 1;
   OCR1A = 260;
   TCCR1B_struct.cs1 = 0x05;
   TIMSK1_struct.ocie1a = 1;


   // Enable interrupts
   system_enable_int();

   print("Hello World\n");

   uint16_t tick_prev;
   uint8_t trig4, count4=0;
   uint8_t state = HUG_SEARCH;
   
   while(1) {
	 tiny_packet_t *p;
	 
	 if (got_packet)
	 if (p = tp_handle('h')) {
		leds_off();
		rf_tx(p->source, 'a', 0);
		state = GOT_HUG;
		tp_clear();
		leds_on();
	}
		

	if (tick!=tick_prev) {
		leds_off();
		
		
		
		count4+=tick-tick_prev; if (count4>15) { trig4=1; count4=0; } else trig4=0;
		
		switch(state) {
			case HUG_SEARCH:
				ANIM_UPDATE(1,1,0);
				anim_flush();
				if (trig4) {
					rf_broadcast('p', 0);
					uint8_t hug_addr;
					if (find_hug(&hug_addr)) {
						print("Hug");
						rf_tx(hug_addr, 'h', 0);
						state = HUG_REQ;
						//break;
					} 
				}
				break;
			case HUG_REQ:
				ANIM_UPDATE(0,0,1);
				anim_flush();
				if (p = tp_handle('a')) {
					state = GAVE_HUG;
					break;
				}
				break;
			case GAVE_HUG:
				ANIM_UPDATE(0,(tick>>2)%12,0);
				anim_flush();	
				break;
			case GOT_HUG:
				ANIM_UPDATE((tick>>2)%12,0,0);
				anim_flush();	
				break;
		}
		tp_clear();
		
		tick_prev = tick;
		leds_on();
	}	
    btns_process();
	leds_process();
   }
}