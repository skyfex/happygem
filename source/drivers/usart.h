#ifndef USART_H
#define USART_H

void usart_init(void);
void usart_sendbyte(char data);
char usart_hasbyte(void);
char usart_getbyte(void);
#define putc usart_sendbyte
void print(char *str);
void print_ushort(unsigned short number);
void print_uchar(unsigned char number);



#endif