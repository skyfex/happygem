
/* ==== RF Driver ====
 * Author: Audun Wilhelmsen
 * 
 *
 */
   
#include "common.h"
#include "drivers/rf.h"
#include "drivers/system.h"
// #include "drivers/usart.h"
#include "include/debug.h"

#include <string.h>
#include <avr/interrupt.h>


#ifdef RF_SNOOP_MODE
#define RF_RX_MODE_CMD     CMD_RX_ON
#define RF_RX_MODE_STATUS  RX_ON
#else
#define RF_RX_MODE_CMD     CMD_RX_AACK_ON
#define RF_RX_MODE_STATUS  RX_AACK_ON
#endif

#define TINY_BUF_SIZE 3
#define TINY_BUF_COUNT 10
#define BIG_BUF_SIZE 128
#define BIG_BUF_COUNT 2


struct {
   rf_rx_handler_t rx_handler;
   uint16_t addr;   // source address
   uint16_t pan_id; // PAN (network) ID
   uint8_t seq_no;  // sequence number
   uint8_t in_seq_no;
   bool got_packet;
   
   rf_packet_t tiny_packet_buf[TINY_BUF_COUNT];
   
   rf_packet_t big_packet_buf[BIG_BUF_COUNT];
   
   uint8_t tiny_data_buf[TINY_BUF_SIZE][TINY_BUF_COUNT];
   
   uint8_t big_data_buf[BIG_BUF_SIZE][BIG_BUF_COUNT];
   
} rf;

ISR(TRX24_TX_END_vect)
{
   dbg_print("Sent packet");
   TRX_STATE_struct.trx_cmd = RF_RX_MODE_CMD;
}
ISR(TRX24_RX_END_vect)
{  
    

    
   rf_packet_t packet;
   uint8_t *buffer = (uint8_t*)&TRXFBST;
   uint8_t length = TST_RX_LENGTH;
   
   // uint8_t FCF0 = buffer[0];
   uint8_t FCF1 = buffer[1];
   
   packet.ed = PHY_ED_LEVEL;
   
   uint8_t data_start = 2; // 2 bytes of FCF
   
   packet.seq = buffer[data_start];
   data_start += 1;
   
   // Parse destination address
   if ((FCF1 & 0b00001100)) { // Has source addr
      packet.has_dest = 1;
      packet.dest_addr = *((uint16_t*)(&buffer[data_start+2]));
      data_start += 4; // 2 bytes of PAN address, 2 bytes of 16-bit address
   }
   else
      packet.has_dest = 0;
 
   // Parse source address
   if ((FCF1 & 0b11000000)) { // Has source addr
      packet.has_source = 1;
      packet.source_addr = *((uint16_t*)(&buffer[data_start+2]));
      data_start += 4; // 2 bytes of PAN address, 2 bytes of 16-bit address
   }
   else
      packet.has_source = 0;
   
   
   packet.data = &buffer[data_start];
   packet.length = length-data_start-2;

   packet.fcs = (buffer[length-2] << 8) | (buffer[length-1]);
   
   packet.in_seq = rf.in_seq_no++;
   
   dbg_print("Got packet");
   
   if (!rf.rx_handler(&packet)) {
      uint8_t i;
      if (packet.length<TINY_BUF_SIZE)
      for (i=0;i<(TINY_BUF_COUNT);i++) {
         if (rf.tiny_packet_buf[i].length==0) {
            memcpy(rf.tiny_data_buf[i], packet.data, packet.length);
            packet.data = rf.tiny_data_buf[i];
            rf.tiny_packet_buf[i] = packet;
            rf.got_packet = 1;
            return;
         }
      }
      for (i=0;i<(BIG_BUF_COUNT);i++) {
         if (rf.big_packet_buf[i].length==0) {
            memcpy(rf.big_data_buf[i], packet.data, packet.length);
            packet.data = rf.big_data_buf[i];
            rf.big_packet_buf[i] = packet;
            rf.got_packet = 1;
            return;
         }
      }
      // TODO: Buffer overflow error
   }
}

void rf_init(uint16_t pan_id, uint16_t addr, rf_rx_handler_t rx_handler)
{
   // Setup config
   rf.rx_handler = rx_handler;
   rf.pan_id = pan_id;
   rf.addr = addr;
   rf.seq_no = 0;
   rf.in_seq_no = 0;
   rf.got_packet = 0;
   
   // Setup registers
   PAN_ID_0 = pan_id&0xFF;
   PAN_ID_1 = pan_id>>8;
   SHORT_ADDR_0 = addr&0xFF;
   SHORT_ADDR_1 = addr>>8;

   // Enable interrupts
   IRQ_MASK_struct.tx_end_en = 1;   
   IRQ_MASK_struct.rx_end_en = 1;
   //IRQ_MASK_struct.pll_lock_en = 1;
   //IRQ_MASK_struct.pll_unlock_en = 1;
   
   // Go to recieve mode
   TRX_STATE_struct.trx_cmd = CMD_TRX_OFF; 
   while(TRX_STATUS_struct.trx_status != TRX_OFF);
   TRX_STATE_struct.trx_cmd = CMD_PLL_ON; 
   while(TRX_STATUS_struct.trx_status != PLL_ON);
   TRX_STATE_struct.trx_cmd = RF_RX_MODE_CMD; 
   while(TRX_STATUS_struct.trx_status != RF_RX_MODE_STATUS);
   dbg_print("RF Initialized");
}

void rf_sleep()
{
   TRXPR_struct.trxrst = 1;
   TRXPR_struct.slptr = 1;
}
void rf_wake()
{
   TRXPR_struct.slptr = 0;
}

void rf_broadcast(uint8_t type, uint8_t data)
{   
   uint8_t buffer[2] = {type, data};
   rf_packet_t packet = {
      .dest_addr = 0xFFFF,
      .length = 2,
      .data = buffer
   };
   rf_transmit(&packet);
}
void rf_broadcast_16(uint8_t type, uint16_t data)
{   
   uint8_t buffer[3] = {type, 0, 0};
   *((uint16_t*)(&buffer[1])) = data;
   rf_packet_t packet = {
      .dest_addr = 0xFFFF,
      .length = 3,
      .data = buffer
   };
   rf_transmit(&packet);
}
void rf_tx(uint16_t addr, uint8_t type, uint8_t data)
{
   uint8_t buffer[2] = {type, data};
   rf_packet_t packet = {
      .dest_addr = addr,
      .length = 2,
      .data = buffer
   };
   rf_transmit(&packet);  
}
void rf_tx_16(uint16_t addr, uint8_t type, uint16_t data)
{
   uint8_t buffer[3] = {type, 0, 0};
   *((uint16_t*)(&buffer[1])) = data;
   rf_packet_t packet = {
      .dest_addr = addr,
      .length = 3,
      .data = buffer
   };
   rf_transmit(&packet);  
}

void rf_transmit(rf_packet_t *packet)
{
   uint8_t status;
   do {
      status = TRX_STATUS_struct.trx_status;
   }     
   while((status == BUSY_TX) ||
         (status == BUSY_TX_ARET) ||
         (status == BUSY_RX) ||
         (status == BUSY_RX_AACK));
   
      TRX_STATE_struct.trx_cmd = CMD_PLL_ON; 
      while(TRX_STATUS_struct.trx_status != PLL_ON) ;
      // print("In PLL_ON state.\n");

      uint8_t length = packet->length + 13; // 2 byte FCF, 1 byte seq no, 4 bytes destination address, 4 bytes source address, ... data ..., 2 byte CRC

      uint8_t *buffer = (uint8_t*)&TRXFBST;
   
      buffer[0] = length;
     
     // Reserved(1), Intra PAN(1), ACK req(1), Frame pend(1), Security(1), Frame type(3)
     // Frame types:: 000: Beacon  001: Data  010: Ack  011: MAC command
      if (packet->req_ack)
         buffer[1] = 0b00100001; // FCF
      else
         buffer[1] = 0b00000001; // FCF
     // Src adr mode(2), Frame ver.(2), Dest adr mode(2), Reserved(2)
     // Adr modes: 00: None  01: Reserved  10: 16-bit  11: 64-bit 
      buffer[2] = 0b10001000; // FCF
      buffer[3] = rf.seq_no++; // seq no
      buffer[4] = rf.pan_id&0xFF;
      buffer[5] = rf.pan_id>>8;
      buffer[6] = packet->dest_addr&0xFF;
      buffer[7] = packet->dest_addr>>8;
      buffer[8] = rf.pan_id&0xFF;
      buffer[9] = rf.pan_id>>8;
      buffer[10] = rf.addr&0xFF; // Use address in packet?
      buffer[11] = rf.addr>>8;

      memcpy(buffer+12, packet->data, packet->length);

      dbg_print("Transmitting packet");

      TRX_STATE_struct.trx_cmd = CMD_TX_ARET_ON;
      while (TRX_STATUS_struct.trx_status != TX_ARET_ON);
      TRX_STATE_struct.trx_cmd = CMD_TX_START; 
      //while(TRX_STATUS_struct.trx_status != BUSY_TX); 
}

bool rf_is_tx_ready()
{
   return !(   (TRX_STATUS_struct.trx_status == BUSY_TX) ||
            (TRX_STATUS_struct.trx_status == BUSY_RX) ||
            (TRX_STATUS_struct.trx_status == BUSY_RX_AACK) );
}

bool rf_handle(uint8_t type, rf_packet_t **packet)
{
   if (!rf.got_packet) return false;
   uint8_t i;
   for (i=0;i<(TINY_BUF_COUNT);i++) {
      if (rf.tiny_packet_buf[i].length!=0) {
         if (rf.tiny_packet_buf[i].data[0] == type) {
            *packet = &rf.tiny_packet_buf[i];
            return true;
         }
      }
   }
   for (i=0;i<(BIG_BUF_COUNT);i++) {
      if (rf.big_packet_buf[i].length!=0) {
         if (rf.big_packet_buf[i].data[0] == type) {
            *packet = &rf.big_packet_buf[i];
            return true;
         }
      }
   }
   return false;
}

bool rf_handle_any(rf_packet_t **packet)
{
   if (!rf.got_packet) return false;
   uint8_t i;
   for (i=0;i<(TINY_BUF_COUNT);i++) {
      if (rf.tiny_packet_buf[i].length!=0) {
         *packet = &rf.tiny_packet_buf[i];
         return true;
      }
   }
   for (i=0;i<(BIG_BUF_COUNT);i++) {
      if (rf.big_packet_buf[i].length!=0) {
         *packet = &rf.big_packet_buf[i];
         return true;
      }
   }
   return false; 
}

void rf_clear_all()
{
   if (!rf.got_packet) return;
   system_disable_int();
   uint8_t i;
   for (i=0;i<(TINY_BUF_COUNT);i++) {
         rf.tiny_packet_buf[i].length = 0;
   }
   for (i=0;i<(BIG_BUF_COUNT);i++) {
         rf.big_packet_buf[i].length = 0;
   }   
   rf.got_packet = false;
   system_enable_int();
}

void rf_clear_old()
{
   if (!rf.got_packet) return;
   system_disable_int();
   uint8_t i;
   for (i=0;i<(TINY_BUF_COUNT);i++) {
      if (rf.in_seq_no - rf.tiny_packet_buf[i].in_seq > 3)
         rf.tiny_packet_buf[i].length = 0;
   }
   for (i=0;i<(BIG_BUF_COUNT);i++) {
      // if (rf.in_seq_no - rf.tiny_packet_buf[i].in_seq > 2)
         rf.big_packet_buf[i].length = 0;
   }   
   rf.got_packet = false;
   system_enable_int();
}


