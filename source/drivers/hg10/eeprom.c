/*
 * eeprom.c
 *
 * Created: 24.01.2012 15:57:51
 *  Author: Audun
 */ 

   
#include "common.h"
#include "drivers/eeprom.h"

void eeprom_write(unsigned int uiAddress, unsigned char ucData) 
{ 

    /* Wait for completion of previous erase/write */ 

    while(EECR & (1<<EEPE)) ;                   
                                                           

    /* Set up address */ 

    EEAR = uiAddress; 

    EEDR = 255; 

    /* Write logical one to EEMPE and enable erase only*/ 

    EECR = (1<<EEMPE) + (1<<EEPM0); 
                                           
    /* Start eeprom erase by setting EEPE */ 

    EECR |= (1<<EEPE); 

    /* Wait for completion of erase */ 

    while(EECR & (1<<EEPE)) ; 

    /* Set up Data Registers */ 
                                                           
    EEDR = ucData;        

    /* Write logical one to EEMPE and enable write only */ 

    EECR = (1<<EEMPE) + (2<<EEPM0); 

    /* Start eeprom write by setting EEPE */ 

    EECR |= (1<<EEPE); 
                                                           
}    

unsigned char eeprom_read(unsigned int uiAddress) 
{                                              
    /* Wait for completion of previous write */ 

    while(EECR & (1<<EEPE)) ;
	
    /* Set up address register */ 

    EEAR = uiAddress; 

    /* Start eeprom read by writing EERE */ 

    EECR |= (1<<EERE);                                                
                                                           
    /* Return data from Data Register */ 

    return EEDR; 

}

void eeprom_erase(unsigned int uiAddress) 
{ 

    /* Wait for completion of previous erase/write */ 

    while(EECR & (1<<EEPE)) ;                   
                                                           

    /* Set up address */ 

    EEAR = uiAddress; 

    EEDR = 255; 

    /* Write logical one to EEMPE and enable erase only*/ 

    EECR = (1<<EEMPE) + (1<<EEPM0); 
                                           
    /* Start eeprom erase by setting EEPE */ 

    EECR |= (1<<EEPE); 

    /* Wait for completion of erase */ 

    while(EECR & (1<<EEPE)) ; 
                         
} 