
#include "misc.h"

void draw_rainbow(pix_t *frame)
{
   uint16_t i;
   {
      pix_t from = (pix_t){{255,0,0}};
      pix_t to = (pix_t){{0,0,255}};
      for (i=0;i<5;i++) {
         frame[i].r = (uint16_t)from.r*(5-i)/5 + (uint16_t)to.r*i/5;
         frame[i].g = (uint16_t)from.g*(5-i)/5 + (uint16_t)to.g*i/5;
         frame[i].b = (uint16_t)from.b*(5-i)/5 + (uint16_t)to.b*i/5;
      }
   }
   {
      pix_t from = (pix_t){{0,0,255}};
      pix_t to = (pix_t){{0,255,0}};
      for (i=0;i<5;i++) {
         frame[5+i].r = (uint16_t)from.r*(5-i)/5 + (uint16_t)to.r*i/5;
         frame[5+i].g = (uint16_t)from.g*(5-i)/5 + (uint16_t)to.g*i/5;
         frame[5+i].b = (uint16_t)from.b*(5-i)/5 + (uint16_t)to.b*i/5;
      }
   }
   {
      pix_t from = (pix_t){{0,255,0}};
      pix_t to = (pix_t){{255,0,0}};
      for (i=0;i<6;i++) {
         frame[10+i].r = (uint16_t)from.r*(6-i)/6 + (uint16_t)to.r*i/6;
         frame[10+i].g = (uint16_t)from.g*(6-i)/6 + (uint16_t)to.g*i/6;
         frame[10+i].b = (uint16_t)from.b*(6-i)/6 + (uint16_t)to.b*i/6;
      }
   }
}