#include <stdbool.h>
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


static inline void admux(bool x)
{
	ADMUX =
	    1 << REFS0 |	/* Vref is AVcc */
	    (x ? ADC_X : ADC_Y);
}


static inline void adcsra(bool start)
{
	/*
	 * The ADC needs to run at clkADC <= 200 kHz for full resolution.
	 * At clkADC = 125 kHz, a conversion takes about 110 us.
	 */
	ADCSRA =
	    1 << ADEN |		/* enable ADC */
	    (start ? 1 << ADSC : 0) |
	    1 << ADIE |         /* enable ADC interrupts */
	    6;                  /* clkADC = clk/64 -> 125 kHz */
}


static uint16_t adc(bool x)
{
	adcsra(0);
	admux(x);
	adcsra(1);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}


int main(void)
{
	uint8_t mode = 0;
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
		if (!PIN(SW_N))
			mode = 0;
		if (!PIN(SW_E))
			mode = 1;
		if (!PIN(SW_S))
			mode = 2;
		switch (mode) {
		case 1:
			send(adc(0));
			break;
		case 2:
			send(adc(1));
			break;
		default:
			send(n);
			n++;
		}
//		_delay_ms(100);
	}
}
