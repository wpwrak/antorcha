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


static volatile uint32_t t_up;	/* uptime, in timer ticks (wraps in 4295 s) */
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
	uint16_t t;

	/* update the time counters */

	t = ICR1;
	t_sw += t;
	t_up += t;

	/* if at the end of the image, only update the time */

	if (curr_line == end_line) {
		PORTB &= 0x3f;
		PORTC = 0;
		PORTD = 0;
		ICR1 = 0xffff;
		sweeping = 0;
		return;
	}

	/* wait before starting the image */

	if (wait_periods) {
		if (!--wait_periods)
			ICR1 = wait_short;
		return;
	}

	/* if done, lights out and slow down */

	/* output the next line */

	PORTB = (PORTB & 0x3f) | (curr_line->cb & 0xc0);
	PORTC = curr_line->cb;
	PORTD = curr_line->d;

	/* move to the next line */

	if (forward)
		curr_line++;
	else
		curr_line--;

	/* wait until the next pixel */

	ICR1 = pixel_ticks;
}


uint32_t uptime_irq(void)
{
	uint32_t t;

	t = t_up+TCNT1;
	if (TIFR1 & TOV1)
		t += ICR1;
	return t;
}


uint32_t uptime(void)
{
	uint32_t a, b;
	uint16_t d;

	do {
		cli();
		a = t_up;
		d = TCNT1;
		cli();
		b = t_up;
		sei();
	}
	while (a != b);
	return a+d;
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

	pixel_ticks = sweep->pixel_ticks;
	wait_periods = sweep->wait_ticks >> 16;
	if (wait_periods) {
	 	wait_short = sweep->wait_ticks;
		wait_period = 0xffff;

		/*
		 * Make sure we have plenty of time to update ICR1 after an
		 * interrupt.
		 */
		if (wait_short < 256) {
			wait_period -= 128;
			wait_short += wait_periods << 7;
		}
		ICR1 = wait_period;
	} else {
	 	ICR1 = sweep->wait_ticks;
	}

	/* prepare the hardware timer */

	t_sw = 0;		/* reset the time */
	TCNT1 = 0;
	TIFR1 = 1 << TOV1;	/* clear interrupt */
	
	/* start the timer */

	TCCR1B =
	    1 << WGM13 |	/* WG Mode 14, continued */
	    1 << WGM12 |
	    1 << CS11;		/* clkIO/8 */

	sweeping = 1;

	sei();
}


void sweep_init(void)
{
	TCCR1A =
	    1 << WGM11;		/* WG Mode 14 (Fast PWM to ICR1) */

	TCNT1 = 0;

	TIMSK1 = 1 << TOIE1;	/* enable timer interrupts */
}
