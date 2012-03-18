
#include "drivers/rf.h"

uint8_t peer_table[16][3];
uint8_t data_table[16];

void peers_rf_handler(packet_t *packet) 
{
	uint8_t data = packet->data[1];
	uint8_t addr = packet->source_addr;
	data_table[addr] = data;
	peer_table[addr][1] = peer_table[addr][0];
	peer_table[addr][0] = packet->ed;
}