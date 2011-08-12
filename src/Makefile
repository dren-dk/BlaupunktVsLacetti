PART=atmega88
F_CPU=8000000

#PROGRAMMER = usbtiny
PROGRAMMER = jtag2isp -P usb

CC=avr-gcc
CFLAGS=-Os -Wall -pedantic-errors -Werror -mcall-prologues -DF_CPU=${F_CPU} -mmcu=${PART} -std=c99 -Wl,-u,vfprintf -lprintf_min

OBJ2HEX=avr-objcopy 

AVRDUDE = avrdude -c ${PROGRAMMER} -p ${PART}

program : bpremote.hex
	$(AVRDUDE) -B 1 -e -U flash:w:bpremote.hex
#	sleep 1;
#	../../hal/util/test-sync ${NODE}

rdfuses:
	$(AVRDUDE) -B 100 -v -q

%.obj : %.asm
	avra -fO $< -o $@

bpremote.obj: bpremote.c blaupunkt-2011.h remote.c remote.h uart.c uart.h Makefile 
	$(CC) $(CFLAGS) -DNODE=$(NODE) uart.c bpremote.c remote.c -o $@
	avr-size --mcu=$(PART) -C $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o

mtest: rfid-manchester-decoder
	./$<

test: remote-test
	./remote-test

remote-test: remote.c remote-test.c remote.h blaupunkt-2011.h 
	gcc -std=c99 -Wall -g3 -pedantic-errors -Werror -o $@ remote.c remote-test.c

blaupunkt-2011.h: lirc2c blaupunkt-2011.lirc
	./lirc2c blaupunkt-2011.lirc blaupunkt-2011.h 
