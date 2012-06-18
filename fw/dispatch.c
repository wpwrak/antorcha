/*
 * fw/dispatch.c - Wireless protocol dispatcher
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

#define F_CPU   8000000UL
#include <util/delay.h>

#include "rf.h"
#include "proto.h"
#include "dispatch.h"
#include "io.h"


static uint8_t seq;	/* last sequence number seen */
static uint8_t type;	/* type currently being processed */
static uint8_t limit;	/* last limit seen */
static const struct handler *curr_proto = NULL;


static void send_ack(const uint8_t *buf)
{
	uint8_t ack[3] = { buf[0]+1, buf[1], 0 };

SET(LED_B6);
	_delay_ms(1);
	rf_send(ack, sizeof(ack));
CLR(LED_B6);
}


static bool answer_ping(const uint8_t *buf)
{
	uint8_t pong[] = { PONG, 0, 0 };

	if (buf[1])
		return 0;
	rf_send(pong, sizeof(pong));
	return 1;
}


bool dispatch(const uint8_t *buf, uint8_t len, const struct handler **protos)
{
SET(LED_B7);
CLR(LED_B7);
	if (len == 3 && buf[0] == PING)
		return answer_ping(buf);

	if (len != 64+3)
		return 0;

	if (!buf[1]) {
		while (*protos) {
			if ((*protos)->type == buf[0])
				break;
			protos++;
		}
		if (!*protos)
			return 0;
		if (!(*protos)->first(buf+3))
			return 0;
		curr_proto = *protos;
		type = buf[0];
		seq = 0;
		limit = buf[2];
		send_ack(buf);
		return 1;
	}

	if (!curr_proto)
		return 0;
	if (buf[0] != type)
		return 0;
	if (buf[1] > limit)
		return 0;
	if (buf[2] != limit)
		return 0;

	if (buf[1] == seq) {
		send_ack(buf);
		return 0;
	}
	if (buf[1] != seq+1)
		return 0;
	if (!curr_proto->more(buf[1], limit, buf+3))
		return 0;
	seq++;
	send_ack(buf);
	return 1;

}
