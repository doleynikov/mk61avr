DEVICE     = atmega328p
CLOCK      = 16000000

AVRDUDE = /home/vitali/Downloads/arduino-1.8.1/hardware/tools/avr/bin/avrdude -C/home/vitali/Downloads/arduino-1.8.1/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyUSB0 -b57600 -D

OBJECTS    = spisramtest.o

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all:	spisramtest.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:spisramtest.hex:i

clean:
	rm -f spisramtest.hex spisramtest.elf $(OBJECTS)

spisramtest.elf: $(OBJECTS)
	$(COMPILE) -o spisramtest.elf $(OBJECTS)

spisramtest.hex: spisramtest.elf
	rm -f spisramtest.hex
	avr-objcopy -j .text -j .data -O ihex spisramtest.elf spisramtest.hex
	avr-size --format=avr --mcu=$(DEVICE) spisramtest.elf
