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
#define F_CPU   8000000UL
#include <util/delay.h>

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
	/*
	 * The ADC needs to run at clkADC <= 200 kHz for full resolution.
	 * At clkADC = 125 kHz, a conversion takes about 110 us.
	 */
	ADCSRA =
	    1 << ADEN |		/* enable ADC */
	    (start ? 1 << ADSC : 0) |
	    1 << ADIE |		/* enable ADC interrupts */
	    6;			/* clkADC = clk/64 -> 125 kHz */
}


uint16_t measure_ref(void)
{
	while (ADCSRA & (1 << ADSC));
	adcsra(0);
	ADMUX =
	    1 << REFS0 |	/* Vref is AVcc */
	    14;			/* Vbg (1.1 V) */
	_delay_us(100);
	adcsra(1);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}


ISR(ADC_vect)
{
	uint16_t v;

	v = ADC;
	if (sample)
		sample(chan_x, v);

	if (chan_x) {
		chan_x = 0;
		admux(0);
		adcsra(1);
	}
}


ISR(TIMER0_OVF_vect)
{
	chan_x = 1;
	admux(1);
	adcsra(1);
}


void accel_start(void)
{
	adcsra(0);

	TCNT0 = 0;
	OCR0A = 125;		/* 8 MHz/64/125 = 1 kHz */
	TCCR0A =
	    1 << WGM01 |	/* WG Mode 7 (Fast PWM to OCR0A) */
	    1 << WGM00;
	TCCR0B =
	    1 << WGM02 |	/* WG Mode 7, continued */
	    1 << CS01 |		/* clkIO/64 */
	    1 << CS00;
	TIMSK0 = 1 << TOIE0;	/* interrupt on overflow */
}
