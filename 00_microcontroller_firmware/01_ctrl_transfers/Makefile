TARGET = usb_blink_control

CFLAGS = -Os -Wall -Wno-unused-but-set-variable -DF_CPU=16000000UL -mmcu=atmega32u4 -Ilib -Wl,-Map=${TARGET}.map -std=gnu11
SOURCES = lib/usb_srs_vendor_v1_2.c main.c

all:
	avr-gcc -o ${TARGET}.elf ${SOURCES} ${CFLAGS}
	avr-objcopy -O ihex ${TARGET}.elf ${TARGET}.hex

clean:
	rm -rf *.hex *.elf *.map
