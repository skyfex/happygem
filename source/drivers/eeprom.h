/*
 * eeprom.h
 *
 * Created: 24.01.2012 15:57:11
 *  Author: Audun
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_


void eeprom_write(unsigned int uiAddress, unsigned char ucData) ;
unsigned char eeprom_read(unsigned int uiAddress) ;
void eeprom_erase(unsigned int uiAddress) ;

#endif /* EEPROM_H_ */