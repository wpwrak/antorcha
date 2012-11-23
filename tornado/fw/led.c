/*
 * fw/led.c - LED control
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

#include "io.h"
#include "led.h"


void led_show(const uint8_t p[LED_BYTES])
{
	uint8_t i;

	for (i = 0; i != 8; i++) {
		while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = p[i];
	}

	/*
	 * There doesn't seem to be any reliable way to synchronize with the
	 * end of the shift operation register that wouldn't cli/sei. Note
	 * that, since the receiver is disabled, we can't use the RX side for
	 * this purpose (as it is done in the atusb firmware).
	 *
	 * So instead we just wait for the worst-case delay, being one byte
	 * in the shift register and another byte in the transmit buffer,
	 * giving a total of 16 bits.
	 *
	 * This is a bit too pessimistic and produces a slight extra delay of
	 * about 1.8 us. (The whole pattern transmission takes 16 us.)
	 * See also ../doc/lclk-1123.png
	 */

	_delay_us(4);	/* 16 bits at 4 MHz */
	SET(LED_LCLK);
	CLR(LED_LCLK);
}


void led_off(void)
{
	static uint8_t zero[LED_BYTES];

	led_show(zero);    
}


void led_init(void)
{
	CLR(LED_SCLK);
	OUT(LED_SCLK);
	CLR(LED_LCLK);
	OUT(LED_LCLK);

	UBRR0 = 0;		/* maximum data rate */
	UCSR0C =
	    1 << UMSEL01 |	/* Master SPI */
	    1 << UMSEL00 | 
	    1 << UDORD0 |	/* MSB first */
	    0 << UCPHA0 |	/* SPI Mode 0 */
	    0 << UCPOL0;
	UCSR0B =
	    1 << TXEN0;		/* enable transmitter */
	UBRR0 = 0;		/* the manual sez we need this */

	led_off();
}
