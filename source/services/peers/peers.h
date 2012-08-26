
#ifndef SERVICES_PEERS_H
#define SERVICES_PEERS_H

#include "drivers/rf.h"


void peers_init(unsigned int addr);
void peers_reset(void);
void peers_process();
void peers_enable();
void peers_disable();
bool peers_rf_handler(rf_packet_t *packet) ;
bool peers_find_hug(uint16_t *addr_out);
void peers_broadcast(uint8_t data);

void peers_do_hug(uint16_t addr);
void peers_unhugged_reset();
char peers_unhugged_in_range();
char peers_unhugged_is_close();

#endif