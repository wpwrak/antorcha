/*
 * fw/sample.c - Acceleration sensor sample protocol
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

#include <avr/interrupt.h>

#include "rf.h"
#include "sweep.h"
#include "accel.h"
#include "proto.h"
#include "dispatch.h"

#include "io.h"

/* @@@ keep it small for now - we're running out of RAM :-( */
//#define	MAX_PACKET	120	/* <- MAX_PSDU -3 (hdr) -2 (CRC) */
#define	MAX_PACKET	50	/* <- MAX_PSDU -3 (hdr) -2 (CRC) */

static uint8_t buf[MAX_PACKET+3] = { SAMPLES, 0, 0 };
static uint16_t *p;
static bool expect_x;


static void handler(bool x, uint16_t v)
{
	uint32_t t;

	if (x != expect_x)
		return;
	t = uptime_irq();
	if (p == (uint16_t *) (buf+3))
		*p++ = t >> 16; 
	*p++ = t;
	*p++ = v;
	expect_x = !expect_x;

	if (x)
		return;
	if ((uint8_t *) (p+4) <= buf+MAX_PACKET)
		return;

	rf_send(buf, (uint8_t *) p-buf);
	buf[1]++;
	p = (uint16_t *) (buf+3);
}


static bool sample_first(uint8_t limit, const uint8_t *payload)
{
	if (payload[0]) {
		buf[1] = 0;
		cli();
		sample = handler;
		p = (uint16_t *) (buf+3);
		expect_x = 1;
		sei();
	} else {
		cli();
		sample = NULL;
		sei();
	}
	return 1;
}


static bool sample_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	return 0;
}


struct handler sample_handler = {
        .type   = SAMPLE,
        .first  = sample_first,
        .more	= sample_more,
};
