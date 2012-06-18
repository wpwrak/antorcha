/*
 * fw/antorcha.c - Initialization of Antorcha application firmware
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stddef.h>
#include <stdint.h>

#include <avr/io.h>
#define F_CPU   8000000UL
#include <util/delay.h>

#include "io.h"
#include "rf.h"
#include "dispatch.h"



static const struct handler *protos[] = {
	NULL
};


int main(void)
{
	uint8_t buf[PAYLOAD+5]; /* 3 bytes header, 2 bytes CRC */
	uint8_t got;

	/*
	 * The boot loader has already initialized PORTx,  DDRx, and MCUCR.PUD.
	 * It has also brought up RF and the underlying SPI.
	 */

while (1) {
	SET(LED_B8);
	_delay_ms(100);
	CLR(LED_B8);
	_delay_ms(100);
}
	while (1) {
		got = rf_recv(buf, sizeof(buf));
		if (got > 2)
			dispatch(buf, got-2, protos);
	}
}
