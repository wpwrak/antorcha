/*
 * fw/accel.c - Acceleration sensor
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
#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "io.h"
#include "accel.h"


void (*sample)(bool x, uint16_t v) = NULL;


static bool chan_x;


static inline void admux(bool x)
{
	ADMUX =
	    1 << REFS0 |	/* Vref is AVcc */
	    (x ? ADC_X : ADC_Y);
}


static inline void adcsra(bool start)
{
	ADCSRA =
	    1 << ADEN |		/* enable ADC */
	    (start ? 1 << ADSC : 0) |
	    1 << ADIE |		/* enable ADC interrupts */
	    7;			/* clkADC = clk/128 -> 62.5 kHz */
}


ISR(ADC_vect)
{
	uint16_t v;

	v = ADC;
	if (sample)
		sample(chan_x, v);

	chan_x = !chan_x;
	admux(chan_x);
	adcsra(1);
}


void accel_start(void)
{
	adcsra(0);
	admux(1);
	chan_x = 1;
}
