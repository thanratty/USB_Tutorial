#ifndef _PHEX_H_
#define _PHEX_H_


#include <stdint.h>


void phexbyte(uint8_t b);
void phexbyte_sp(uint8_t b);

void phexword(uint16_t w);
void phexword_sp(uint16_t w);

void pcrlf( void );

#endif

