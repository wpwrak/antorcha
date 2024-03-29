/*
 * fw/boot.c - Antorcha boot loader
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
#include <avr/pgmspace.h>

#include "io.h"
#include "rf.h"
#include "proto.h"
#include "dispatch.h"
#include "fw.h"


#define	MS_TO_LOOP(ms)	((uint32_t) (ms)*106)


static void wait_upload(void)
{
	uint8_t buf[PAYLOAD+5]; /* 3 bytes header, 2 bytes CRC */
	uint32_t i;
	uint8_t got;

restart:
	for (i = 0; i != MS_TO_LOOP(5000); i++) {
		got = rf_recv(buf, sizeof(buf));
		if (got > 2 && fw_packet(buf, got-2))
			goto restart;
	}

}


int main(void)
{
	volatile int zero = 0;

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

	/* Disable pull-ups */
	MCUCR |= 1 << PUD;

	/*
	 * Disable the watchdog timer, in case the boot loader has been
	 * entered by a watchdog reset commanded by the application.
	 */

	MCUSR = 0;		/* Remove override */
	WDTCSR |= 1 << WDCE;	/* Enable change */
	WDTCSR = 1 << WDCE;	/* Disable watchdog while still enabling
				   change */

	rf_init();

	/*
	 * Switch the LED inside the loop so that we get a short pulse one can
	 * observe on a scope.
	 */
	do {
		SET(LED_B8);
		wait_upload();
		CLR(LED_B8);
	} while (pgm_read_byte(zero) == 0xff);

	((void (*)(void)) 0)();

	/* not reached */
	return 0;
}
