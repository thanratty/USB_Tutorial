#include <avr/io.h>
#include "uart.h"



void phexbyte(uint8_t b)
{
    uint8_t ch;

    ch = (b >> 4) & 0x0f;
    ch += ((ch>9) ? 55 : 48);
    uart1_putc(ch);
    ch = (b & 0x0f);
    ch += ((ch>9) ? 55 : 48);
    uart1_putc(ch);
}


void phexword(uint16_t w)
{
    phexbyte(w >> 8);
    phexbyte(w & 0xFF);
}


void phexword_sp(uint16_t w)
{
    phexword(w);
    uart1_putc(' ');
}

void phexbyte_sp(uint8_t b)
{
    phexbyte(b);
    uart1_putc(' ');
}


void pcrlf( void )
{
    uart1_putc(13);
    uart1_putc(10);
}

