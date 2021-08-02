PRG	= main
OBJ = ${PRG}.o

BAUDRATE        = 57600
HZ              = 16000000
MCUTARGET       = atmega328p
AVRDUDE_TARGET  = atmega328p
PORT            = /dev/ttyUSB0
PROGRAMMER      = arduino

CC   = avr-gcc
OPT  = -O3
DEFS =
LIBS =

override CFLAGS  = -g -std=c99 -DF_CPU=$(HZ) -Wall $(OPT) -mmcu=$(MCUTARGET) $(DEFS)
override LDFLAGS = -Wl,-Map,$(PRG).map

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size

all: $(PRG).elf lst text

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o $(PRG).elf *.eps *.png *.pdf *.bak *.hex *.bin *.srec *.lst *.map

lst: $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

text: hex bin srec
hex: $(PRG).hex
bin: $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	$(SIZE) --mcu=$(MCUTARGET) --format=avr $<

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

install: $(PRG).hex
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) -b $(BAUDRATE) -v -U flash:w:$(PRG).hex

screen:
	screen $(PORT)

.PHONY: all clean install srec bin hex text lst
