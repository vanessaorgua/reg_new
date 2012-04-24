
MCU=atmega32

CSRC=main.c lcd.c delay.c work.c net_drv.c \
setup.c setup_channel.c setup_system.c setup_calibr.c

# spi.c
TARGET = reg_new

F_OSC = 11059200
BASE = 16

CC=avr-gcc
LD = avr-gcc
AS = avr-gcc
AVROBJ = avr-objcopy
OBJDUMP = avr-objdump


CCFLAGS = -g -Wall -Os
#,-adhlns=$(<:.c=.lst)
CCFLAGS += -DF_OSC=$(F_OSC) -funsigned-char 
CCFLAGS += -DBASE=$(BASE)
#CCFLAGS	+= -Os 

LDFLAG = -g -Wl,-Map,$(TARGET).map
#LDFLAG += -u,vfprintf -lprintf_flt -lm

comma = ,
empty =
space = $(empty) $(empty)

OBJLIST  = $(CSRC:.c=.o)
OBJECTS	= $(subst $(space),$(comma),$(OBJLIST) )




all: hex lss size



prog: $(TARGET).hex
	avrdude -p m32 -P /dev/ttyS0 -c ponyser -U flash:w:$(TARGET).hex -U eeprom:w:$(TARGET)_eeprom.hex

eeread:
	avrdude -p m32 -P /dev/ttyS0 -c ponyser -U eeprom:r:$(TARGET)_eeprom.hex

fuse: lfuse.bin hfuse.bin
	avrdude -p m32 -P /dev/ttyS0 -c ponyser -U lfuse:w:lfuse.bin:r -U hfuse:w:hfuse.bin:r
  
#prog: $(TARGET).hex
#	loader $(TARGET).hex

lss:  $(TARGET).lss

hex:  $(TARGET).hex

size: $(TARGET).hex
	avr-size $(TARGET).out

ehex:  $(TARGET)_eeprom.hex


%_eeprom.hex: %.out
	$(AVROBJ) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

	
$(TARGET).out : $(OBJLIST)
	$(LD) -o $(TARGET).out $(OBJLIST) $(LDFLAG) -mmcu=$(MCU)

%o: %c
	$(CC)  -c $<  $(CCFLAGS) -mmcu=$(MCU)
	
%.lss: %.out
	$(OBJDUMP) -h -S $< > $@
	
%hex: %out
		$(AVROBJ) -j .text -j .data -O ihex $< $@
		
#main.o: main.c main.h
		
clean:
	rm -f *.o *out *lst *lss *.hex *.map

