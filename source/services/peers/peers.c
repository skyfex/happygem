
#include "peers.h"

#include "drivers/all.h"

// Range value in double of actual measurements
// (Compared to sum of two last measurements)
#define PRS_HUG_RANGE 80
#define PRS_CLOSE_RANGE 30
#define PRS_SIZE 16
// Timing values in 64ths seconds
#define PRS_TIMEOUT 128    
#define PRS_SLOW_PING_DELAY 64
#define PRS_MID_PING_DELAY 32
#define PRS_FAST_PING_DELAY 4

uint16_t peers_addr[PRS_SIZE];
uint8_t peers_timeout[PRS_SIZE];
uint8_t peers_range[PRS_SIZE][2];
uint8_t peers_data[PRS_SIZE];

static uint16_t last_hug_addr;
static uint8_t ping_timer;
static uint8_t enabled;
static unsigned int eeprom_addr;
static uint8_t unhugged_in_range;
static uint8_t unhugged_is_close;


bool peers_rf_handler(rf_packet_t *packet) 
{
   if (!enabled) return true;
   uint8_t i;
   uint8_t found = 0;
   uint8_t min_range = 255;
   uint8_t min_i = 0;

   // print("r: "); print_ushort(packet->source_addr); print(" "); print_uchar(packet->ed); print("\n");

   // Look for existing or free slot in peers table
   for (i=0; i<PRS_SIZE; i++) {
      if (peers_addr[i] == packet->source_addr) {
         found = 1;
         break;
      }
      if (peers_timeout[i] == 0) {
         found = 1;
         peers_range[i][0] = 0;
         break;
      }
      if (peers_range[i][0] < min_range) {
         min_i = i;
         min_range = peers_range[i][0];
         peers_range[i][0] = 0;
      }
   }
   // Address was not in peers table, replace
   // the slot with minimum range
   if (!found) {
      if (packet->ed > min_range) {
         i = min_i;
      }     
      else {
         return true;
      }
   }
   // print("i: "); print_uchar(i); print("\n");
   peers_timeout[i] = 128;
   peers_addr[i] = packet->source_addr;
   peers_range[i][1] = peers_range[i][0];
   peers_range[i][0] = packet->ed;
   peers_data[i] = packet->data[1];
	return true;
}


void peers_reset()
{
   uint8_t i;
   for(i=0; i<16; i++) {
      peers_addr[i] = 0;
      peers_timeout[i] = 0;
      peers_range[i][0] = 0;
      peers_range[i][1] = 0;
   }                 
}

void peers_enable()
{
   peers_reset();
   enabled = 1;
}

void peers_disable()
{
   enabled = 0;
}

void peers_init(unsigned int addr)
{
   eeprom_addr = addr;
   last_hug_addr = 0;
   ping_timer = 0;
   peers_reset();
   enabled = 1;
}

bool peers_find_hug(uint16_t *addr_out)
{
   uint8_t i;
   for(i=0; i<PRS_SIZE; i++) {
      if (last_hug_addr != peers_addr[i])
      if ((peers_range[i][0]+peers_range[i][1]) > PRS_HUG_RANGE) {
         *addr_out = peers_addr[i];
         return true;
      }
   }
   return false;
}

void peers_do_hug(uint16_t addr)
{
   if (addr > 1024) return;

   unsigned int rom_addr = eeprom_addr + addr;

   eeprom_write(rom_addr, 1);
}

char peers_is_hugged(uint16_t addr)
{
   if (addr > 1024) return true;

   unsigned int rom_addr = eeprom_addr + addr;

   return (eeprom_read(rom_addr) == 1);
}

char peers_unhugged_in_range()
{
   return unhugged_in_range;
}

char peers_unhugged_is_close()
{
   return unhugged_is_close;
}


void peers_process()
{
   if (!enabled) return;
   uint8_t i;
   char is_in_range = 0;
   char is_close = 0;

   if (tick64) {
      unhugged_is_close = 0;
      unhugged_in_range = 0;
      for (i=0; i<PRS_SIZE; i++) {
         if (peers_timeout[i] != 0) {
            peers_timeout[i]--;
            char is_hugged = peers_is_hugged(peers_addr[i]);
            is_in_range = 1;
            if (!is_hugged) unhugged_in_range = 1;
            if (peers_range[i][0] > PRS_CLOSE_RANGE/2) {
               if (!is_hugged) unhugged_is_close = 1;
               is_close = 1;
            }
         }
      }

      ping_timer++;
      if ((ping_timer > PRS_SLOW_PING_DELAY) ||
          (is_in_range && ping_timer > PRS_MID_PING_DELAY) ||
          (is_close && ping_timer > PRS_FAST_PING_DELAY)) {

         // if (is_close) print("p c\n");
         // else if (is_in_range) print("p r\n");
         // else print("p s\n");

         ping_timer = 0;
         peers_broadcast(0);
      }
   }
}

void peers_broadcast(uint8_t data)
{
	rf_broadcast('p', data);
}



