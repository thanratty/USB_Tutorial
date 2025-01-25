/* Copyright (c)   2022 Johannes 4Linux  johannes[at]gnu-linux[dot]rocks
                   2013 Guy WEILER       weigu[at]weigu[dot]lu

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Licence: GPL-3        */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/usb_srs_vendor_v1_2.h"

#include "leds.h"
#include "uart.h"




#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s


// Test button wired to INT1 pin
#define		BTEST_PORT_DATA		PORTD
#define		BTEST_PORT_DDR		DDRD
#define		BTEST_BIT			1			// INT1 / SDA



extern uint8_t button_counter;
extern bool leds_on;						// Sent from host




void InitTestButton( void )
{
	CBI(DDRD, BTEST_BIT);		// Input
	SBI(PORTD, BTEST_BIT);	// Pull-up

    EICRA = (1 << ISC11) | (0 << ISC10);  	// b'10 interrupt on falling edge of INT1 pin
    EIMSK = (1 << INT1);  					// Enable INT1 external interrupt
}



void InitTimer0( void )
{
	// b'[8:7] = COM0A1:COM0A0 = 00, normal operation. Pin OC0A disconnected
	// b'[1:0] = WGM01:WGM00 = 10, Clear timer on compare
	TCCR0A  = 0x02;

	// b'7 FOC0A = 1		Force output compare A 
	// b'[2:0] = CS02:CS01:CS00 = 101. Prescale = 1024, so 16 MHz/1024 = 16,000 = 62.5 uS/tick
	// So 128 ticks = 8 ms exactly
	TCCR0B  = 0x85;

	// Output Compare A = 128
	OCR0A   = 128;

	// Enable Output Compare Match Interrupr
	SBI(TIMSK0, OCIE0A);

	// Zero counter
	TCNT0   = 0x00;
}




/*
Vector #22 @ 0x002A
*/
 ISR(TIMER0_COMPA_vect)
{
static uint16_t int_count = 0;


	// Every 8 ms, so 125 = 1 second
	int_count++;

	if (int_count >= 125)
	{
//		TX_LED_TOGGLE;
		int_count = 0;
	}

}




/*
Vector #3 @ 0x0004
*/
 ISR(INT1_vect)
{
//	TX_LED_TOGGLE;
	button_counter++;
}



void InitLEDs( void )
{
	// Init LED GPIO
	RX_LED_DDR |= (1 << RX_LED_BIT);
	TX_LED_DDR |= (1 << TX_LED_BIT);

	RX_LED_OFF;
	TX_LED_OFF;
}




// L"\\\\?\\usb#vid_8477&pid_3118#1234#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"



int main(void)
{
	// Configure for 16 MHz clock, prescaler = 0
	CLKPR = (1<<CLKPCE);  
	CLKPR = 0;

	InitLEDs();
	RX_LED_OFF;
	TX_LED_OFF;

	InitTestButton();

	uart1_init(0x0019);
	_delay_ms(100);
	uart1_puts("\033[2J\nReady.\r");
	uart1_flush();
	_delay_ms(1000);

	// Start USB
	usb_init_device();

	InitTimer0();

	uint8_t loop_count = 0;
	while (1)
	{
		_delay_ms(1000);
		RX_LED_TOGGLE;

// 		uart1_putc('A'+loop_count);
		loop_count++;
		if (loop_count==26)
			loop_count=0;
	}
}

