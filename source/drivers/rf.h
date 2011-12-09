#ifndef DRIVERS_RF_H
#define DRIVERS_RF_H

#include <include/types.h>

uint8_t length;

typedef struct {
   uint8_t has_source;
   uint16_t source_addr;
   uint8_t has_dest;
   uint16_t dest_addr;
   
   uint8_t length;
   uint8_t *data;
   
   uint8_t seq;
   uint8_t ed;
   } rf_packet_t;

typedef void (*rf_rx_handler_t)(rf_packet_t *packet);


void rf_init(uint16_t pan_id, uint16_t addr, rf_rx_handler_t rx_handler);
void rf_broadcast(uint8_t type, uint8_t data);
void rf_tx(uint16_t addr, uint8_t type, uint8_t data);


#endif