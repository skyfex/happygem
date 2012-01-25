
#include "drivers/usart.h"

//#define DEBUG_MODE

#ifdef DEBUG_MODE
#define dbg_print(X) print(X);putc('\n');
#define DEBUG(X) X
#else
#define dbg_print(X) 
#define DEBUG(X)
#endif