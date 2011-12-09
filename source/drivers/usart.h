
void usart_init(void);
void usart_sendbyte(char data);
#define putc usart_sendbyte
void print(char *str);
void print_ushort(unsigned short number);
void print_uchar(unsigned char number);