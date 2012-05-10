#ifndef DRIVERS_RF_H
#define DRIVERS_RF_H

#include <include/types.h>

uint8_t length;

typedef struct {
   uint8_t req_ack;
   
   uint8_t has_source;
   uint16_t source_addr;
   uint8_t has_dest;
   uint16_t dest_addr;
   
   uint8_t length;
   uint8_t *data;
   
   uint8_t seq;      // Remote sequence number, incremented for each sent packet
   uint16_t fcs;     // Frame control sequence (CRC Checksum)
   uint8_t ed;       // Energy detection (signal strength)
   
   uint8_t in_seq;   // A local sequence number, incremented for each incoming packet
   } rf_packet_t;

typedef bool (*rf_rx_handler_t)(rf_packet_t *packet);


void rf_init(uint16_t pan_id, uint16_t addr, rf_rx_handler_t rx_handler);
void rf_transmit(rf_packet_t *packet);
bool rf_is_tx_ready();

void rf_broadcast(uint8_t type, uint8_t data);
void rf_broadcast_16(uint8_t type, uint16_t data);
void rf_tx(uint16_t addr, uint8_t type, uint8_t data);
void rf_tx_16(uint16_t addr, uint8_t type, uint16_t data);

bool rf_handle(uint8_t type, rf_packet_t **packet);
bool rf_handle_any(rf_packet_t **packet);
void rf_clear_all();
void rf_clear_old();

void static inline rf_done(rf_packet_t *packet)
{
   packet->length = 0;
}

#endif