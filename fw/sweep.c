/*
 * fw/image.c - Image sweep
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "image.h"
#include "sweep.h"


volatile bool sweeping = 0;


static volatile uint32_t t_sw;	/* cumulative number of timer ticks in sweep */

static uint16_t wait_periods;	/* number of periods to wait before image */
static uint16_t wait_period;	/* ticks in wait period */
static uint16_t wait_short;	/* ticks to wait after periods */

static uint16_t pixel_ticks;	/* number of ticks per pixel */
static const struct line *curr_line;
static const struct line *end_line;
static bool forward;


ISR(TIMER1_OVF_vect)
{
	/* update the sweep time */

	t_sw += OCR1A;

	/* if at the end of the image, only update the time */

	if (curr_line == end_line)
		return;

	/* wait before starting the image */

	if (wait_periods) {
		if (!--wait_periods)
			OCR1A = wait_short;
		return;
	}

	/* output the next line */

	PORTB = (PORTB & 0x3f) | (curr_line->cb & 0xc0);
	PORTC = curr_line->cb;
	PORTD = curr_line->d;

	/* move to the next line */

	if (forward)
		curr_line++;
	else
		curr_line--;

	/* wait until the next pixel (or slow down if we're done) */

	if (curr_line == end_line) {
		OCR1A = 0xffff;
		sweeping = 0;
	} else {
		OCR1A = pixel_ticks;
	}
}


void sweep_image(const struct sweep *sweep)
{
	TCCR1B = 0;	/* stop the timer */

	cli();

	/* shut down all the LEDs, in case we're still in a sweep */

	PORTB &= 0x3f;
	PORTC = 0;
	PORTD = 0;

	/* image pointers and sweep direction */

	forward = sweep->forward;
	if (forward) {
		curr_line = image+sweep->left;
		end_line = image+sweep->right+1;
	} else {
		curr_line = image+sweep->right;
		end_line = image+sweep->left-1;
	}

	/* timing parameters */

	wait_periods = sweep->wait_ticks >> 16;
	if (wait_periods) {
	 	wait_short = sweep->wait_ticks;
		wait_period = 0xffff;

		/*
		 * Make sure we have plenty of time to update OCR1A after an
		 * interrupt.
		 */
		if (wait_short < 256) {
			wait_period -= 128;
			wait_short += wait_periods << 7;
		}
		OCR1A = wait_period;
	} else {
	 	OCR1A = sweep->wait_ticks;
	}

	/* prepare the hardware timer */

	t_sw = 0;		/* reset the time */
	TCNT1 = 0;
	TIFR1 = 1 << TOV1;	/* clear interrupt */
	
	/* start the timer */

	TCCR1B =
	    1 << WGM13 |	/* WG Mode 15, continued */
	    1 << WGM12 |
	    1 << CS11;		/* clkIO/8 */

	sweeping = 1;

	sei();
}


void sweep_init(void)
{
	TCCR1A =
	    1 << WGM11 |	/* WG Mode 15 (Fast PWM to OCR1A) */
	    1 << WGM10;

	TCNT1 = 0;

	TIMSK1 = 1 << TOIE1;	/* enable timer interrupts */
}
