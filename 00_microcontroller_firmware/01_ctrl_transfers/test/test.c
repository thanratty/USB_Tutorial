#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
//#include <avr/interrupt.h>

#include "uart.h"

int main(void)
{
	// 38.4 kB
	uint16_t divisor = 0x0019;
	//avar = UART_BAUD_SELECT(F_CPU,38400);

	while(1)
	{
		uart1_init(divisor);
		uart1_puts("Hello world\n");
	}

	return 0;
}


