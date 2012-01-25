	

NAME=happygem

GCC_PART=atmega128rfa1
GCC_FLAGS=-Wall -Os -mmcu=$(GCC_PART)

DUDE_PART=m128rfa1
DUDE_PROGRAMMER=dragon_jtag
DUDE_FLAGS=-c $(DUDE_PROGRAMMER) -P usb -p $(DUDE_PART) 

FW=fire_grass_water
# DRIVERS=hg01
# SERVICES=
# APPS=
include fw/$(FW)/Makefile.inc

SERVICES_C_FILES=$(shell ls ./services/anim/*.c)
DRIVER_C_FILES=$(shell ls ./drivers/$(DRIVERS)/*.c)
FW_C_FILES=$(shell ls ./fw/$(FW)/*.c)
C_FILES=$(DRIVER_C_FILES) $(SERVICES_C_FILES) $(FW_C_FILES) fw/main.c

INCLUDES=./

default: $(C_FILES)
	@avr-gcc $(GCC_FLAGS) -I$(INCLUDES) -o outputs/$(FW).elf $(C_FILES)
	@avr-objcopy -j .text -j .data -O ihex outputs/$(FW).elf outputs/$(FW).hex
	
program: default
	avrdude $(DUDE_FLAGS) -U flash:w:outputs/$(FW).hex
	
readfuse:
	avrdude $(DUDE_FLAGS) -U lfuse:r:fuses/low.hex:h
	sleep 3
	avrdude $(DUDE_FLAGS) -U hfuse:r:fuses/high.hex:h
	sleep 3
	avrdude $(DUDE_FLAGS) -U efuse:r:fuses/ext.hex:h
	
writefuse:
	avrdude $(DUDE_FLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0x19:m -U efuse:w:0xfe:m
			
clean:
	rm terminoutputs/*
	