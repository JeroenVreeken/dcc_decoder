VPATH= src
CFLAGS+= -Isrc

TARGET=avr
MCU=atmega328p
CC=${TARGET}-gcc
OBJCOPY=${TARGET}-objcopy
CFLAGS+= -Wall -Werror -O1
CFLAGS+= -MT $@ -MMD -MP -MF $*.d
CFLAGS+= -mmcu=$(MCU) -DF_CPU=16000000UL 


LDFLAGS=  -mmcu=${MCU} 
LDFLAGS+= -Xlinker -Map=accessory_servo.map 
LDFLAGS+= ${CFLAGS}



include build.mk

all: accessory_servo.hex


accessory_servo: accessory_servo.o dcc_decoder.o pwm.o stub.o

accessory_servo.hex: accessory_servo

clean:
	rm -rf *.o accessory_servo *.d *hex *map
	make -f Makefile_ut clean

program: accessory_servo.hex
	sudo avrdude -c usbasp -p m328p -U flash:w:accessory_servo.hex


test:
	make -f Makefile_ut
	./ut_dcc_decoder

-include *.d

