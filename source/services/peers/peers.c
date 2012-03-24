
#include "peers.h"

uint8_t peer_table[16][2];
uint8_t data_table[16];

bool peers_rf_handler(rf_packet_t *packet) 
{
	uint8_t data = packet->data[1];
	uint8_t addr = packet->source_addr;
	data_table[addr] = data;
	peer_table[addr][1] = peer_table[addr][0];
	peer_table[addr][0] = packet->ed;
	return true;
}

bool peers_find_hug(uint8_t *addr_out, uint8_t hug_range, uint8_t data)
{
   for(uint8_t i=0; i<16; i++) {
      if ((peer_table[i][0]+peer_table[i][1]) > hug_range) {
         if (data_table[i] == data) {
            *addr_out = i;
            return true;
         }        
      }
   }
   return false;
}

void peers_reset()
{
   for(uint8_t i=0; i<16; i++) {
      peer_table[i][0] = 0;
      peer_table[i][1] = 0;
   }                 
}

void peers_broadcast(uint8_t data)
{
	rf_broadcast('p', data);
}