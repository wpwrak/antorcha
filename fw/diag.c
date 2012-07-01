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
	uint8_t pkg[7] = { DIAG_ACK, 0, 0, };
	uint16_t v;

	cli();
	set_line(localize_line(tmp[0], tmp[1]));
	v = measure_ref(1);
	pkg[3] = v;
	pkg[4] = v >> 8;
	v = measure_ref(0);
	pkg[5] = v;
	pkg[6] = v >> 8;
	set_line(localize_line(0, 0));
	sei();
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
		if (failed)
			return 0;
		do_diag();
		break;
	}
	/* do_diag sends the ACK, not the dispatcher */
	return 0;
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
