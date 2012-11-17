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
	uint8_t last = 0xff;
	uint8_t sw = 0xff;

	PORTB = HIGH(B);
	PORTC = HIGH(C);
	PORTD = HIGH(D);
	DDRB = OUTPUTS(B);
	DDRC = OUTPUTS(C);
	DDRD = OUTPUTS(D);

#if 0
	PORTB = 0xff;
	PORTC = 0xff;
	PORTD = 0xff;
#endif

	CLR(CARD_nPWR);
	CLR(SCLK);
	CLR(LCLK);
	OUT(SCLK);
	OUT(LCLK);
	OUT(DS);

	SET(VDD);
	OUT(VDD);

#if 0
	while (1) {
		SET(SCLK);
		CLR(SCLK);
	}
#endif

#if 0
	while (1) {
		uint8_t v = 0;

		v = PIN(SW_N) | PIN(SW_E) << 1 | PIN(SW_S) << 2 |
		    PIN(SW_W) << 3 | PIN(SW_SW) << 4;
		if (v != sw) {
			sw = v;
			n = 0;
			continue;
		}
		n++;
		if (n < 10)
			continue;
		if (!(sw & 1))
			SET(DS);
		if (!(sw & 4))
			CLR(DS);
		if (!(sw & 2))
			SET(SCLK);
		if (!(sw & 8))
			CLR(SCLK);
		if (sw & 16)
			CLR(LCLK);
		else
			SET(LCLK);
		last = sw;
	}
#endif

#if 1
	while (1) {
		send(n);
		n++;
//		_delay_ms(100);
#endif
	}
}
