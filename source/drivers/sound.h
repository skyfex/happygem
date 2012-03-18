#ifndef SOUND_H
#define SOUND_H

#include <include/types.h>


#define HZ(x) ((uint16_t)(2000000/x/4))

#define C2	   HZ( 65.41 ) 
#define Cs2    HZ(69.30  )
#define D2	   HZ( 73.42 ) 
#define Ds2    HZ(77.78  )
#define E2	   HZ( 82.41 ) 
#define F2	   HZ( 87.31 ) 
#define Fs2    HZ(92.50  )
#define G2	   HZ( 98.00 ) 
#define Gs2    HZ(103.83 )
#define A2	   HZ( 110.00) 
#define As2    HZ(116.54 )
#define B2	   HZ( 123.47) 

#define C3	   HZ( 130.81) 
#define Cs3    HZ(138.59 )
#define D3	   HZ( 146.83) 
#define Ds3    HZ(155.56 )
#define E3	   HZ( 164.81) 
#define F3	   HZ( 174.61) 
#define Fs3    HZ(185.00 )
#define G3	   HZ( 196.00) 
#define Gs3    HZ(207.65 )
#define A3	   HZ( 220.00) 
#define As3    HZ(233.08 )
#define B3	   HZ( 246.94) 

#define C4	 HZ(261.63)
#define Cs4	 HZ(277.18)
#define D4	 HZ(293.66)
#define Ds4	 HZ(311.13)
#define E4	 HZ(329.63)
#define F4	 HZ(349.23)
#define Fs4	 HZ(369.99)
#define G4	 HZ(392.00)
#define Gs4	 HZ(415.30)
#define A4	 HZ(440.00)
#define As4	 HZ(466.16)
#define B4	 HZ(493.88)

#define C5	 HZ(523.25)
#define Cs5	 HZ(554.37)
#define D5	 HZ(587.33)
#define Ds5	 HZ(622.25)
#define E5	 HZ(659.26)
#define F5	 HZ(698.46)
#define Fs5	 HZ(739.99)
#define G5	 HZ(783.99)
#define Gs5	 HZ(830.61)
#define A5	 HZ(880.00)
#define As5	 HZ(932.33)
#define B5	 HZ(987.77)

typedef struct {
   uint16_t val;
   uint8_t dur; 
} sound_t;

typedef struct {
   uint8_t count;
   sound_t *sounds;
   uint8_t delay;
   uint8_t repeat;
} sound_pattern_t;

void sound_init();
void sound_play(sound_pattern_t *pattern);
void sound_process();

#endif