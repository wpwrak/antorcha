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

#include "proto.h"
#include "param.h"
#include "rf.h"
#include "dispatch.h"
#include "sweep.h"
#include "accel.h"
#include "image.h"

#include <avr/interrupt.h>
#include "io.h"


static volatile enum sync_state {
	IDLE,	/* undecided */
	BWD,	/* backward sweep */
	LEFT,	/* reversing from backward to forward */
	FWD,	/* forward sweep */
	RIGHT,	/* reversing from forward to backward */
} state = IDLE;

static volatile uint32_t tR0, tR1, tL0, tL1;
static volatile uint32_t tL, tR;
static volatile bool wake = 0;


static void sync_sweep(bool x, uint16_t v)
{
	uint32_t t;

	if (!x)
		return;
	t = uptime_irq();
	switch (state) {
	case IDLE:
		if (v < xa_low) {
			tR0 = t;
			state = RIGHT;
		} else if (v > xa_high) {
			tL0 = t;
			state = LEFT;
		}
		break;
	case RIGHT:
		if (v < xa_low)
			break;
		tR1 = t;
		tR = t-tR0;
		state = BWD;
		/* fall through */
	case BWD:
		if (v < xa_high)
			break;
		tL0 = t;
		state = LEFT;
		wake = 1;
		break;
	case LEFT:
		if (v > xa_high)
			break;
		tL1 = t;
		tL = t-tL0;
		state = FWD;
		/* fall through */
	case FWD:
		if (v > xa_low)
			break;
		tR0 = t;
		state = RIGHT;
		wake = 1;
		break;
	}
}


static void submit_fwd_sweep(void)
{
#if 0
	uint32_t tIMG;

	tIMG = (fwd_sweep.right-fwd_sweep.left+1)*(fwd_sweep.pixel_ticks)/2;
	fwd_sweep.start_ticks = tL0+110000-tIMG/2;
#endif
	fwd_sweep.start_ticks = tL0+fwd_start;
	sweep_image(&fwd_sweep);
}


static void submit_bwd_sweep(void)
{
	bwd_sweep.start_ticks = tR0+bwd_start;
	sweep_image(&bwd_sweep);
}


static const struct handler *protos[] = {
	&image_handler,
	&reset_handler,
	&sample_handler,
	&param_handler,
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

	sweep_init();
	sample = sync_sweep;
	accel_start();
	sei();

	while (1) {
		got = rf_recv(buf, sizeof(buf));
		if (got > 2)
			dispatch(buf, got-2, protos);
		if (wake) {
			wake = 0;
			if (state == LEFT)
				submit_fwd_sweep();
			else if (state == RIGHT)
				submit_bwd_sweep();
		}
	}
}
