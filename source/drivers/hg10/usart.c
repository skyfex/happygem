

#include "common.h"
#include "drivers/usart.h"

#define BAUD USART_BAUD
#include <util/setbaud.h>

// TODO: Move these two to a utility library?
char ushort_to_str_result[6];
char *ushort_to_str(unsigned short number)
{
   int i=5;
   ushort_to_str_result[i] = 0;
   do {
      i--;
      ushort_to_str_result[i] = '0'+(number%10);
      number = (number/10);
   } while(number!=0);
   return &ushort_to_str_result[i];
}

char uchar_to_str_result[4];
char *uchar_to_str(unsigned char number)
{
   int i=3;
   uchar_to_str_result[i] = 0;
   do {
      i--;
      uchar_to_str_result[i] = '0'+(number%10);
      number = (number/10);
   } while(number!=0);
   return &uchar_to_str_result[i];
}

void usart_init(void){
	// Default frame format: 8 data bits, no parity, 1 stop bit
   
   // Set baud rate
   UBRR0L = UBRRL_VALUE; 
   UBRR0H = UBRRH_VALUE;

  // Enable receiver and transmitter and receive complete interrupt 
   UCSR0B = ((1<<TXEN0) |(1<<RXEN0));// | (1<<RXCIE0));
}


void usart_sendbyte(char data){

  // Wait until last byte has been transmitted
  while((UCSR0A &(1<<UDRE0)) == 0);

  // Transmit data
  UDR0 = data;
}

char usart_hasbyte(void) {
   return UCSR0A & (1<<RXC0);
}

char usart_getbyte( void ) {
   /* Wait for data to be received */
   while ( !(UCSR0A & (1<<RXC0)) );
   return UDR0;
}

void print(char *str)
{
	char c;
	while ((c=*(str++))!=0) { // C drunken kung-fu
		usart_sendbyte(c);
	}
}
void print_ushort(unsigned short number)
{
   print(ushort_to_str(number));
}
void print_uchar(unsigned char number)
{
   print(uchar_to_str(number));
}