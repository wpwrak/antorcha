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


#define	MAX_PACKET	120	/* <- MAX_PSDU -3 (hdr) -2 (CRC) */

static uint8_t buf[MAX_PACKET+3] = { SAMPLES, 0, 0 };
static uint16_t *p;


static void handler(bool x, uint16_t v)
{
	bool first;
	uint32_t t;

	first = p == (uint16_t *) (buf+3);
	if (first && !x)
		return;
	t = uptime();
	if (first)
		*p++ = t >> 16; 
	*p++ = t;
	*p++ = v;
	if (x)
		return;
	if ((uint8_t *) (p+4) <= buf+MAX_PACKET)
		return;
	rf_send(buf, (uint8_t *) p-buf);
	p = (uint16_t *) (buf+3);
}


static bool sample_first(uint8_t limit, const uint8_t *payload)
{
	if (payload[0]) {
		buf[1] = 0;
		cli();
		sample = handler;
		p = (uint16_t *) (buf+3);
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
