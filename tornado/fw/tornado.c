#include <stdbool.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#define F_CPU   8000000UL
#include <util/delay.h>

#include "io.h"
#include "led.h"
#include "mmc.h"
#include "accel.h"


#define	HIGH(port) \
	(MASK(port, CARD_nPWR) | \
	MASK(port, SW_N) | MASK(port, SW_E) | MASK(port, SW_S) | \
	MASK(port, SW_W) | MASK(port, SW_SW))

#define	OUTPUTS(port) \
	(MASK(port, CARD_nPWR) | MASK(port, CARD_CLK) | \
	MASK(port, LED_DS) | MASK(port, LED_LCLK) | MASK(port, LED_SCLK))


#if 0

/*
 * @@@ For testing, connect the LED bar via the 8:10 card slot, so that it
 * can be disconnected without soldering.
 */

#define	SCLK	CARD_DAT1
#define	LCLK	CARD_DAT0
#define	DS	CARD_CLK
#define	VDD	CARD_CMD

#else

#define	SCLK	LED_SCLK
#define	LCLK	LED_LCLK
#define	DS	LED_DS

#endif


static void send(uint8_t pattern[N_LEDS/8])
{
	uint8_t i, j, mask;

	for (i = 0; i != N_LEDS/8; i++) {
		mask = 1;
		for (j = 0; j != 8; j++) {
			if (pattern[i] & mask)
				SET(DS);
			else
				CLR(DS);
			SET(SCLK);
			CLR(SCLK);
			mask <<= 1;
		}
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


#define	E_SHIFT	8	/* ~ 0.06 */
#define	M_SHIFT	11	/* ~ 2/sample_rate */

#define	HYSTERESIS	9	/* 1 g / 3 */


static const uint8_t img[] PROGMEM = {
	#include "img.inc"
};


static uint8_t one[LED_BYTES] =
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static volatile uint16_t sample_t = 0, sample_v;


static void zxing(uint16_t x, uint16_t y)
{
	static uint32_t e = (uint32_t) 512 << E_SHIFT;
	static uint32_t m = (uint32_t) 512 << M_SHIFT;
	int16_t d;
	static bool up = 0;
	static bool on = 0;
	static const prog_uint8_t *p;
	static uint16_t cols = 0;

	sample_t++;
	sample_v = x;

	e = x+(e-(e >> E_SHIFT));
	m = x+(m-(m >> M_SHIFT));
	d = (e >> E_SHIFT)-(m >> M_SHIFT);
	if (up) {
		if (d < -HYSTERESIS)
			up = 0;
	} else {
		if (d > HYSTERESIS) {
			up = 1;
			p = img;
			cols = sizeof(img)/LED_BYTES;
			on = 1;
		}
	}
	if (cols) {
		led_show_pgm(p);
		p += 8;
		cols--;
	} else {
		led_off();
	}
}


static void panic(void)
{
	cli();
	while (1) {
		led_show(one);
		_delay_ms(100);
		led_off();
		_delay_ms(100);
	}
}


int main(void)
{
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

#ifdef VDD
	SET(VDD);
	OUT(VDD);
#endif

	led_init();

#if 0
	led_show(one);

	if (!mmc_init())
		panic();
	if (!mmc_begin_write(0))
		panic();

	uint16_t n = 0;

	for (n = 0; n != 512; n += 2) {
		mmc_write(n);
		mmc_write(n >> 8);
	}

	if (!mmc_end_write())
		panic();

	if (!mmc_begin_write(n))
		panic();

	for (; n != 1024; n += 2) {
		mmc_write(n);
		mmc_write(n >> 8);
	}

	if (!mmc_end_write())
		panic();

	_delay_ms(1000);

	led_off();

	while (1);
#endif

#if 1
	uint16_t last_t = 0;
	uint32_t n = 0;

	sample = zxing;
	/* MMC doesn't work when running from battery, probably because we
	   have no regulation. Just disable it for now. */
#if 0
	if (!mmc_init())
		panic();
#endif
	accel_start();
	sei();
while (1);
	while (1) {
		uint16_t t, v;

		if (!(n & 511)) {
			if (n && !mmc_end_write())
				panic();
			if (!mmc_begin_write(n))
				panic();
		}

#if 0
	t = n;
	v = 0;
#else
		do {
			cli();
			t = sample_t;
			v = sample_v;
			sei();
		}
		while (t == last_t);
#endif

		last_t = t;
		mmc_write(t);
		mmc_write(t >> 8);
		mmc_write(v);
		mmc_write(v >> 8);
		n += 4;
	}
#endif

#if 0
	static uint8_t p[LED_BYTES];
	uint8_t mode = 0;
	uint16_t n = 0, v;

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
			n = adc(0);
			p[0] = n;
			p[1] = n >> 8;
			p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = 0;
			send(p);
			break;
		case 2:
			n = adc(1);
			p[0] = n;
			p[1] = n >> 8;
			p[2] = p[3] = p[4] = p[5] = p[6] = p[7] = 0;
			send(p);
			break;
		default:
			v = 63-n;
			if (n & 64)
				p[(v >> 3) & 7] &= ~(1 << (v & 7));
			else
				p[(v >> 3) & 7] |= 1 << (v & 7);
			led_show(p);
			n++;
		}
		_delay_ms(100);
	}
#endif
}
