#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
#include <stdlib.h>
// #include <util/delay.h>
void win_anim(uint16_t tick);
void loose_anim(uint16_t tick);
void stage0(uint16_t tick);
void stage1(uint16_t tick);
void stage2(uint16_t tick);
void stage3(uint16_t tick);
void stage4(uint16_t tick);
void stage5(uint16_t tick);

enum States { HUG_SEARCH, HUG_REQ, GOT_HUG, GAVE_HUG, GAME_OVER };

#define HUG_RANGE 130

uint8_t gem_id;
bool rand_initialized = 0;

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
uint8_t data_table[16];

uint16_t tick = 0;

ISR(TIMER1_COMPA_vect)
{   
	tick++;
}

bool find_hug(uint8_t *addr_out, uint8_t data)
{
	for(uint8_t i=0; i<16; i++) {
		if ((peer_table[i][0]+peer_table[i][1])>HUG_RANGE) {
			if (data_table[i]==data) {
				*addr_out = i;
				return 1;
			}			
		}
	}
	return 0;
}
void peers_reset()
{
	for(uint8_t i=0; i<16; i++) {
		peer_table[i][0] = 0;
		peer_table[i][1] = 0;
		peer_table[i][2] = 0;
	}						
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
	if (!rand_initialized) { srand(tick); rand_initialized = 1; }
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
   uint8_t addr = packet->source_addr;
   if (type=='p') {
	   //peer_table[addr][2] = peer_table[addr][1];
	   data_table[addr] = data;
	   peer_table[addr][1] = peer_table[addr][0];
	   peer_table[addr][0] = packet->ed;
   }
   else {
	   //if (tp_buf[tp_ind].type) dbg_print("Buffer overflow");
	   tp_buf[tp_ind].source = addr;
	   tp_buf[tp_ind].type = type;
	   tp_buf[tp_ind].data = data;
	   tp_ind = (tp_ind+1)%16;
	   got_packet = 1;
   }
   //putc(type); print(" "); print_uchar(data); print(" "); print_uchar(packet->ed); print("\n");
}


void fw_main()
{
	//eeprom_write(0, 3);
	gem_id = eeprom_read(0);
	
   // Init drivers
   system_init();
   usart_init();
   print("(((O))) HappyGem #"); print_uchar(gem_id); print(" (((O)))\n");
   
   btns_init(btn_handler);                                                                                 
   rf_init(1337, gem_id, rf_rx_handler);
 
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

   uint16_t tick_prev = tick;
   uint8_t trig4, count4=0;
	short countdown = 0;
   uint8_t state = HUG_SEARCH;
   uint8_t did_win = 0;
   signed char level = 0;
   
   while(1) {
	 tiny_packet_t *p;
	 
	 if (got_packet) {
		 leds_off();
		 if ((p = tp_handle('h'))) {
			
			did_win = !p->data;
			rf_tx(p->source, 'a', p->data);
			peers_reset();
			if (did_win) level++;
			else level--;
			if (level<0) level=0;
			state = GOT_HUG;
			countdown = 90;
			tp_clear();
		}
		//if (state!=GAME_OVER)
		if ((p = tp_handle('g'))) {
			did_win = 0;
			level = 0;
			state = GAME_OVER;
			tp_clear();
		}
		leds_on();
	 }	
		
	if (tick!=tick_prev) {
		leds_off();

		uint8_t delta = tick-tick_prev;
		count4+=tick-tick_prev; if (count4>15) { trig4=1; count4=0; } else trig4=0;
		countdown -= delta;
		
		switch(state) {
			case HUG_SEARCH:
				switch(level) {
					case 0: stage0(tick); break;
					case 1: stage1(tick/10); break;
					case 2: stage2(tick/10); break;
					case 3: stage3(tick/10); break;
					case 4: stage4(tick/10); break;
				}
				anim_flush();
				
				if (trig4) {
					rf_broadcast('p', level);
					uint8_t hug_addr;
					if (find_hug(&hug_addr, level)) {
	
						did_win = rand()%2;
						if (did_win) level++;
						else level--;
						if (level<0) level=0;
						rf_tx(hug_addr, 'h', did_win);
						peers_reset();
						state = HUG_REQ;
						countdown = 480;
						//break;
					} 
				}
				break;
			case HUG_REQ:
				ANIM_UPDATE(0,0,1);
				anim_flush();
				if ((p = tp_handle('a'))) {
					state = GOT_HUG;
					countdown = 90;
					break;
				}
				if (countdown<0) {
					state = HUG_SEARCH;
				}
				break;
			case GOT_HUG:  
				if (did_win) {
					win_anim(tick);
				}				
				else {
					loose_anim(tick);
				}			
				anim_flush();			
				if (countdown<0) {
					if (level==5) {
						//tick = 120; tick_prev = 120;
						did_win = 1;
						state= GAME_OVER;
						break;
					}
					state = HUG_SEARCH;
				}
				break;
			case GAME_OVER: 
		
				if (did_win) {
					rf_broadcast('g',0);
					stage5(tick);
				}				
				else {
					ANIM_UPDATE(0,0,0);
				}
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