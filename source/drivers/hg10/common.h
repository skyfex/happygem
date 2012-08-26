
// ==== Configuration ====
#define F_CPU 16000000
#define USART_BAUD 28800
#define DBG_MODE

#include <avr/io.h>

// ==== I/O Pin Utility Functions ====
#define P0 (1<<0)
#define P1 (1<<1)
#define P2 (1<<2)
#define P3 (1<<3)
#define P4 (1<<4)
#define P5 (1<<5)
#define P6 (1<<6)
#define P7 (1<<7)

#define enablePinOutput(PLETTER, PNUM) DDR##PLETTER |= (PNUM)
#define enablePinInput(PLETTER, PNUM) DDR##PLETTER &= ~(PNUM)
#define enablePinPullup(PLETTER, PNUM) PORT##PLETTER |= (PNUM)
#define disablePinPullup(PLETTER, PNUM) PORT##PLETTER &= ~(PNUM)
#define setPin(PLETTER, PNUM) PORT##PLETTER |= (PNUM)
#define clrPin(PLETTER, PNUM) PORT##PLETTER &= ~(PNUM)
#define tglPin(PLETTER, PNUM) PORT##PLETTER ^= (PNUM)
#define getPin(PLETTER, PNUM) (PIN##PLETTER & PNUM)
