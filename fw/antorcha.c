/*
 * fw/main.c - Initialization of Antorcha firmware
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <avr/io.h>

#include "io.h"
#include "rf.h"


int main(void)
{
	/* Port B has two LEDs and the rest is SPI */
	PORTB = MASK(B, RF_nSS) | MASK(B, RF_nRST);
	DDRB = MASK(B, RF_SCLK) | MASK(B, RF_MOSI) | MASK(B, RF_nSS) |
	    MASK(B, RF_nRST) | 0xc0;

	/* All port C pins drive LEDs */
	PORTC = 0;
	DDRC = 0x3f;

	/* All port D pins drive LEDs */
	PORTD = 0;
	DDRD = 0xff;

	/* disable pull-ups */
	MCUCR |= 1 << PUD;

	rf_init();

	while (0) {
		PORTC = 2;
		PORTC = 0;
		rf_send("HOLA ?", 6);
	}

	while (1) {
		uint8_t buf[1], got;

		do {
PORTC = 1;
PORTC = 0;
got = rf_recv(buf, 1);
}
		while (!got);

PORTC = 2;
PORTC = 0;
		PORTD = buf[0];
	}
}
