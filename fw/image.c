/*
 * fw/image.c - Image data (upload and conversion)
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

#include "io.h"
#include "hash.h"
#include "proto.h"
#include "dispatch.h"
#include "secret.h"
#include "image.h"


static struct line images[2][MAX_LINES];
static const struct line *next_image;
static struct line *p, *end;
static bool failed;

const struct line *image = images[0];


#define	MAP(port, value, group) ( \
    ((value) & 1 ? MASK(port, LED_##group##1) : 0) | \
    ((value) & 2 ? MASK(port, LED_##group##2) : 0) | \
    ((value) & 4 ? MASK(port, LED_##group##3) : 0) | \
    ((value) & 8 ? MASK(port, LED_##group##4) : 0) | \
    ((value) & 16 ? MASK(port, LED_##group##5) : 0) | \
    ((value) & 32 ? MASK(port, LED_##group##6) : 0) | \
    ((value) & 64 ? MASK(port, LED_##group##7) : 0) | \
    ((value) & 128 ? MASK(port, LED_##group##8) : 0))


static void add_payload(const uint8_t *payload)
{
	uint8_t i, b, c, d;

	for (i = 0; i != PAYLOAD && p != end; i += 2) {
		b = MAP(B, payload[i], A) | MAP(B, payload[i+1], B);
		c = MAP(C, payload[i], A) | MAP(C, payload[i+1], B);
		d = MAP(D, payload[i], A) | MAP(D, payload[i+1], B);
		p->d = d;
		p->cb = c | b;
		p++;
	}
}


static bool image_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	switch (limit-seq) {
	default:
		add_payload(payload);
		/* fall through */
	case 3:
	case 2:
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
		image = next_image;
		break;
	}
	return 1;
}


static bool image_first(uint8_t limit, const uint8_t *payload)
{
	hash_init();
	hash_merge_progmem(image_secret, sizeof(image_secret));
	if (image == images[0])
		p = images[1];
	else
		p = images[0];
	next_image = p;
	end = p+MAX_LINES;
	memset(p, 0, (char *) end-(char *) p);
	failed = 0;
	return image_more(0, limit, payload);;
}


struct handler image_handler = {
        .type   = IMAGE,
        .first  = image_first,
        .more	= image_more,
};
