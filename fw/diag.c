/*
 * fw/diag.c - Diagnostic request
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
#include <string.h>

#include <avr/interrupt.h>
#define	F_CPU	8000000UL
#include <util/delay.h>

#include "hash.h"
#include "proto.h"
#include "rf.h"
#include "dispatch.h"
#include "secret.h"
#include "image.h"
#include "sweep.h"
#include "accel.h"


static uint8_t tmp[2];
static bool failed;


static void do_diag(void)
{
	uint8_t pkg[3+2*DIAG_SAMPLES] = { DIAG_ACK, 4, 0, };
	uint8_t *p = pkg+3;
	uint16_t v;
	uint8_t i;

	cli();
	set_line(localize_line(tmp[0], tmp[1]));
	_delay_ms(50);
	for (i = 0; i != DIAG_SAMPLES; i++) {
		v = measure_ref();
		*p++ = v;
		*p++ = v >> 8;
	}
	set_line(localize_line(0, 0));
	/*
	 * @@@ for some reason we get a difference of 100 mV between passes
	 * if we enable interrupts here
	 *
	 * sei();
	 */
	rf_send(pkg, sizeof(pkg));
}


static bool diag_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	switch (limit-seq) {
	default:
		hash_merge(payload, PAYLOAD);
		break;
	case 1:
		hash_end();
		failed = !hash_eq(payload, PAYLOAD, 0);
		break;
	case 0:
		if (!hash_eq(payload, PAYLOAD, PAYLOAD))
			failed = 1;
		if (!failed)
			do_diag();
		/* do_diag sends the ACK, not the dispatcher */
		return 0;
	}
	return 1;
}


static bool diag_first(uint8_t limit, const uint8_t *payload)
{
	hash_init();
	hash_merge_progmem(image_secret, sizeof(image_secret));
	hash_merge(payload, PAYLOAD);
	memcpy(&tmp, payload, sizeof(tmp));
	failed = 0;
	return 1;
}


struct handler diag_handler = {
        .type   = DIAG,
        .first  = diag_first,
        .more	= diag_more,
};
