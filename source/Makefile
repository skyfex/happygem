	

NAME=happygem

GCC_PART=atmega128rfa1
GCC_FLAGS=-Wall -Os -mmcu=$(GCC_PART) -std=c99 -fms-extensions

DUDE_PART=m128rfa1
DUDE_PROGRAMMER=dragon_jtag
DUDE_FLAGS=-c $(DUDE_PROGRAMMER) -P usb -p $(DUDE_PART) 

FW?=happygem10
# DRIVERS=hg01
# SERVICES=
# APPS=
include fw/$(FW)/Makefile.inc

ifdef ID
	GCC_FLAGS+=-DGEM_ID=$(ID)
endif

GCC_FLAGS+=-DDRIVERS_$(DRIVERS)

SERVICES_C_FILES=$(shell for i in $(SERVICES) ; do ls ./services/$$i/*.c ; done)
DRIVER_C_FILES=$(shell ls ./drivers/$(DRIVERS)/*.c)
FW_C_FILES=$(shell ls ./fw/$(FW)/*.c)
C_FILES=$(DRIVER_C_FILES) $(SERVICES_C_FILES) $(FW_C_FILES) fw/main.c

INCLUDES=.\


default: $(C_FILES)
	mkdir -p outputs
	@avr-gcc $(GCC_FLAGS) -I$(INCLUDES) -o outputs/$(FW).elf $(C_FILES)
	@avr-objcopy -j .text -j .data -O ihex outputs/$(FW).elf outputs/$(FW).hex
	
program: default
	avrdude $(DUDE_FLAGS) -U flash:w:outputs/$(FW).hex
	
readfuse:
	mkdir -p fuses
	avrdude $(DUDE_FLAGS) -U lfuse:r:fuses/low.hex:h -U hfuse:r:fuses/high.hex:h  -U efuse:r:fuses/ext.hex:h
	# sleep 3
	# avrdude $(DUDE_FLAGS) 
	# sleep 3
	# avrdude $(DUDE_FLAGS)
	
writefuse:
	avrdude $(DUDE_FLAGS) -U lfuse:w:0xf7:m -U hfuse:w:0x11:m -U efuse:w:0xfe:m

init:
	avrdude $(DUDE_FLAGS) -U flash:w:outputs/$(FW).hex -U lfuse:w:0xf7:m -U hfuse:w:0x11:m -U efuse:w:0xfe:m	
	# sleep 2
	# avrdude $(DUDE_FLAGS) 
		
stop:
	avrdude $(DUDE_FLAGS)  -U flash:w:stop.hex
reset:
	avrdude $(DUDE_FLAGS)	
clean:
	rm terminoutputs/*
	
