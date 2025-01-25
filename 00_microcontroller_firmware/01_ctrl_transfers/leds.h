#ifndef _LEDS_H_
#define _LEDS_H_



#define		RX_LED_PORT		PORTB
#define     RX_LED_DDR      DDRB
#define		RX_LED_BIT		0

#define		TX_LED_PORT		PORTD
#define     TX_LED_DDR      DDRD
#define		TX_LED_BIT		5



#define		RX_LED_OFF		do { RX_LED_PORT |=  (1 << RX_LED_BIT); } while(0)
#define		RX_LED_ON		do { RX_LED_PORT &= ~(1 << RX_LED_BIT); } while(0)

#define		TX_LED_OFF		do { TX_LED_PORT |=  (1 << TX_LED_BIT); } while(0)
#define		TX_LED_ON		do { TX_LED_PORT &= ~(1 << TX_LED_BIT); } while(0)

#define     RX_LED_TOGGLE   do { RX_LED_PORT ^= (1 << RX_LED_BIT); } while(0)
#define     TX_LED_TOGGLE   do { TX_LED_PORT ^= (1 << TX_LED_BIT); } while(0)



#endif
