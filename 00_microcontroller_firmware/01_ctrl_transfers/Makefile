TARGET = usb_blink_control

CFLAGS = -Os -Wall -Wextra -Wno-unused-but-set-variable -DF_CPU=16000000UL -mmcu=atmega32u4 -Ilib -I. -Wl,-Map=${TARGET}.map -std=gnu11
SOURCES = main.c lib/descriptors.c lib/usb_srs_vendor_v1_2.c uart.c 

all:
	avr-gcc -o ${TARGET}.elf ${SOURCES} ${CFLAGS}
	avr-objcopy -O ihex ${TARGET}.elf ${TARGET}.hex

clean:
	rm -rf *.hex *.elf *.map *.s *.lst
