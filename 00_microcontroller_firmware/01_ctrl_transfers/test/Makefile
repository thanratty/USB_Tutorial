TARGET = hello

CFLAGS = -Os -Wall -DF_CPU=16000000UL -mmcu=atmega32u4 -I. -Wl,-Map=${TARGET}.map -std=gnu11
SOURCES = test.c uart.c

all:
	avr-gcc ${CFLAGS} -S test.c 
	avr-gcc ${CFLAGS} -o ${TARGET}.elf ${SOURCES} 
	avr-objcopy -O ihex ${TARGET}.elf ${TARGET}.hex

clean:
	rm -rf *.hex *.elf *.map *.s *.lst
