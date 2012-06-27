/*
 * fw/param.c - Parameter upload
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

#include <avr/pgmspace.h>

#include "hash.h"
#include "sweep.h"
#include "image.h"
#include "proto.h"
#include "dispatch.h"


struct sweep fwd_sweep = {
	.pixel_ticks	= TP_FWD_PIX_DEFAULT,
	.left		= PX_FWD_LEFT_DEFAULT,
	.right		= PX_FWD_RIGHT_DEFAULT,
	.forward	= 1,
};


struct sweep bwd_sweep = {
	.pixel_ticks	= TP_BWD_PIX_DEFAULT,
	.left		= PX_BWD_LEFT_DEFAULT,
	.right		= PX_BWD_RIGHT_DEFAULT,
	.forward	= 0,
};

uint16_t xa_high = XA_HIGH_DEFAULT;
uint16_t xa_low = XA_LOW_DEFAULT;
uint32_t fwd_start = TP_FWD_START_DEFAULT;
uint32_t bwd_start = TP_BWD_START_DEFAULT;

static struct params tmp;
static bool failed;


static void new_params(void)
{
	xa_high = tmp.xa_high;
	xa_low = tmp.xa_low;

	fwd_sweep.left = tmp.px_fwd_left;
	fwd_sweep.right = tmp.px_fwd_right;
	bwd_sweep.left = tmp.px_bwd_left;
	bwd_sweep.right = tmp.px_bwd_right;

	fwd_start = tmp.tp_fwd_start;
	bwd_start = tmp.tp_bwd_start;
	fwd_sweep.pixel_ticks = tmp.tp_fwd_pix;
	bwd_sweep.pixel_ticks = tmp.tp_bwd_pix;
}


static bool param_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
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
		new_params();
		break;
	}
	return 1;
}


static bool param_first(uint8_t limit, const uint8_t *payload)
{
	hash_init();
	hash_merge_progmem(image_secret, sizeof(image_secret));
	hash_merge(payload, PAYLOAD);
	memcpy(&tmp, payload, sizeof(tmp));
	failed = 0;
	return 1;
}


struct handler param_handler = {
        .type   = PARAM,
        .first  = param_first,
        .more	= param_more,
};
