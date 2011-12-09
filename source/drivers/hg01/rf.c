
/* ==== RF Driver ====
 * Author: Audun Wilhelmsen
 * 
 *
 */
   
#include "common.h"
#include "drivers/rf.h"
#include "include/debug.h"

#include <avr/interrupt.h>


struct {
   rf_rx_handler_t rx_handler;
   uint16_t addr;   // source address
   uint16_t pan_id; // PAN (network) ID
   uint8_t seq_no;  // sequence number
} rf;

ISR(TRX24_TX_END_vect)
{
   dbg_print("Sent packet");
   TRX_STATE_struct.trx_cmd = CMD_RX_AACK_ON;
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
   if ((FCF1 & 0b00001100) == 0b00001000) { // Has source addr
      packet.has_dest = 1;
      packet.dest_addr = *((uint16_t*)(&buffer[data_start+2]));
      data_start += 4; // 2 bytes of PAN address, 2 bytes of 16-bit address
   }
   else
      packet.has_source = 0;
 
   // Parse source address
   if ((FCF1 & 0b11000000) == 0b10000000) { // Has source addr
      packet.has_source = 1;
      packet.source_addr = *((uint16_t*)(&buffer[data_start+2]));
      data_start += 4; // 2 bytes of PAN address, 2 bytes of 16-bit address
   }
   else
      packet.has_source = 0;
   
   
   packet.data = &buffer[data_start];
   packet.length = length-data_start-2;
   
   dbg_print("Got packet");
   rf.rx_handler(&packet);
   // uint8_t length = TST_RX_LENGTH;
   // uint8_t ed = PHY_ED_LEVEL;
   // rf.rx_handler(length, buffer+2, ed);
}

void rf_init(uint16_t pan_id, uint16_t addr, rf_rx_handler_t rx_handler)
{
   // Setup config
   rf.rx_handler = rx_handler;
   rf.pan_id = pan_id;
   rf.addr = addr;
   rf.seq_no = 0;
   
   // Setup registers
   PAN_ID_0 = pan_id&0xFF;
   PAN_ID_1 = pan_id>>8;
   SHORT_ADDR_0 = addr&0xFF;
   SHORT_ADDR_1 = addr>>8;

   // Enable interrupts
   IRQ_MASK_struct.tx_end_en = 1;   
   IRQ_MASK_struct.rx_end_en = 1;

   // Go to recieve mode
   TRX_STATE_struct.trx_cmd = CMD_TRX_OFF; 
   while(TRX_STATUS_struct.trx_status != TRX_OFF);
   TRX_STATE_struct.trx_cmd = CMD_RX_AACK_ON; 
   
   dbg_print("RF Initialized");
}

void rf_broadcast(uint8_t type, uint8_t data)
{   
   rf_tx(0xFFFF, type, data);
}

void rf_tx(uint16_t addr, uint8_t type, uint8_t data)
{
   while((TRX_STATUS_struct.trx_status == BUSY_TX) ||
         (TRX_STATUS_struct.trx_status == BUSY_RX) ||
         (TRX_STATUS_struct.trx_status == BUSY_RX_AACK));
   
      TRX_STATE_struct.trx_cmd = CMD_PLL_ON; 
      while(TRX_STATUS_struct.trx_status != PLL_ON);
      // print("In PLL_ON state.\n");

      uint8_t length = 2 + 13; // 2 byte FCF, 1 byte seq no, 4 bytes destination address, 4 bytes source address, ... data ..., 2 byte CRC

      uint8_t *buffer = (uint8_t*)&TRXFBST;
   
      buffer[0] = length;
      buffer[1] = 0b00000001; // FCF
      buffer[2] = 0b10001000; // FCF
      buffer[3] = rf.seq_no++; // seq no
      buffer[4] = rf.pan_id&0xFF;
      buffer[5] = rf.pan_id>>8;
      buffer[6] = addr&0xFF;
      buffer[7] = addr>>8;
      buffer[8] = rf.pan_id&0xFF;
      buffer[9] = rf.pan_id>>8;
      buffer[10] = rf.addr&0xFF;
      buffer[11] = rf.addr>>8;

      buffer[12] = type;
      buffer[13] = data;

      TRX_STATE_struct.trx_cmd = CMD_TX_START; 
      // while(TRX_STATUS_struct.trx_status != BUSY_TX); 
}

// uint8_t rf_rx(uint8_t *ed_ptr)
// {
//    // print("Going to RX state\n");
//    while((TRX_STATUS_struct.trx_status != PLL_ON) && (TRX_STATUS_struct.trx_status != RX_ON));
//    TRX_STATE_struct.trx_cmd = CMD_RX_ON; 
//    while(TRX_STATUS_struct.trx_status != RX_ON);
//    
// 
//    if (IRQ_STATUS_struct.rx_end)
//    {
//       IRQ_STATUS_struct.rx_end = 1;
//       uint8_t ed = PHY_ED_LEVEL;
//       uint8_t *buffer = (uint8_t*)&TRXFBST;
//       uint8_t data = buffer[3];
//       // print("Got packet\n");
//       // print_uchar(ed); print("\n");
//       *ed_ptr = ed;
//       return 1;
//    }
//    else
//       return 0;
// }