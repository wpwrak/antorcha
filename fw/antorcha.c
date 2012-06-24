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

#include "rf.h"
#include "dispatch.h"
#include "sweep.h"
#include "accel.h"
#include "image.h"

#include <avr/interrupt.h>
#include "io.h"


static struct sweep sweep = {
	.wait_ticks	=	 60000,	/*  60 ms */
	.pixel_ticks	=	  1000,	/*   1 ms */
	.left		=	     0,
	.right		=	    79,
	.forward 	=	     1,
};


static volatile enum sync_state {
	IDLE,
	LEFT_DECEL,
	LEFT_ACCEL,
	FWD_START_SWEEP,
	FWD_SWEEP,
} state = IDLE;


#define	LEFT_DECEL_THRESH	850
#define	LEFT_REVERSE_THRESH	900
#define	LEFT_ACCEL_THRESH	850
#define	RIGHT_DECEL_THRESH	200


static void sync_sweep(bool x, uint16_t v)
{
	static uint32_t t;

	if (!x)
		return;
	switch (state) {
default:
	case IDLE:
		if (v < LEFT_DECEL_THRESH)
			break;
		state = LEFT_DECEL;
		t = uptime_irq();
		break;
	case LEFT_DECEL:
		if (v < LEFT_DECEL_THRESH) {
			t = uptime_irq();
			break;
		}
#if 0
		if (v > LEFT_REVERSE_THRESH)
			state = LEFT_ACCEL;
		break;
	case LEFT_ACCEL:
		if (v > LEFT_ACCEL_THRESH)
			break;
#else
		if (v < LEFT_REVERSE_THRESH)
			break;
#endif
		if (sweeping) { /* we're confused */
			state = IDLE;
			break;
		}
		state = FWD_START_SWEEP;
		/* t = t1-(t1+t0)/2 = (t1-t0)/2 */
		t = (uptime_irq()-t) >> 1;
//		sweep.wait_ticks = kkkkkk
		break;	
	case FWD_START_SWEEP:
		break;
	case FWD_SWEEP:
		if (v > RIGHT_DECEL_THRESH)
			break;
		state = IDLE;
		break;
	}
}


static const struct handler *protos[] = {
	&image_handler,
	&reset_handler,
	&sample_handler,
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
#if 1
		if (state == FWD_START_SWEEP && !sweeping) {
			state = FWD_SWEEP;
			sweep_image(&sweep);
		}
#endif
	}
}
