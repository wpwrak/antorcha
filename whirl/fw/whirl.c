#include <stdint.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "io.h"


#define	HIGH(port) \
	(MASK(port, CARD_nPWR) | \
	MASK(port, SW_N) | MASK(port, SW_E) | MASK(port, SW_S) | \
	MASK(port, SW_W) | MASK(port, SW_SW))

#define	OUTPUTS(port) \
	(MASK(port, CARD_nPWR) | MASK(port, CARD_CLK) | \
	MASK(port, LED_DS) | MASK(port, LED_LCLK) | MASK(port, LED_SCLK))


/*
 * @@@ For testing, connect the LED bar via the 8:10 card slot, so that it
 * can be disconnected without soldering.
 */

#define	SCLK	CARD_DAT1
#define	LCLK	CARD_DAT0
#define	DS	CARD_CLK
#define	VDD	CARD_CMD


static void send(uint16_t pattern)
{
	uint8_t i;

	for (i = 0; i != 16; i++) {
		if (pattern & 0x8000)
			SET(DS);
		else
			CLR(DS);
		SET(SCLK);
		CLR(SCLK);
		pattern <<= 1;
	}
	SET(LCLK);
	CLR(LCLK);
}


int main(void)
{
	uint16_t n = 0;

	PORTB = HIGH(B);
	PORTC = HIGH(C);
	PORTD = HIGH(D);
	DDRB = OUTPUTS(B);
	DDRC = OUTPUTS(C);
	DDRD = OUTPUTS(D);

	CLR(CARD_nPWR);
	CLR(SCLK);
	CLR(LCLK);
	OUT(SCLK);
	OUT(LCLK);
	OUT(DS);

	SET(VDD);
	OUT(VDD);

	while (1) {
		while (!PIN(SW_SW));
		send(n);
		n++;
//		_delay_ms(100);
	}
}
