
#ifndef SERVICES_PEERS_H
#define SERVICES_PEERS_H

#include "drivers/rf.h"

bool peers_rf_handler(rf_packet_t *packet) ;
bool peers_find_hug(uint8_t *addr_out, uint8_t hug_range, uint8_t data);
void peers_reset(void);
void peers_broadcast(uint8_t data);

#endif