DEVICE     = atmega328p
CLOCK      = 16000000

AVRDUDE = <Copy here a complete output string from Arduino-IDE. To do that enable 'Show verbose output during upload' in Arduino IDE Preferences>

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
