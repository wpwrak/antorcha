/*
 * fw/fw.h - Firmware upload protocol
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
#include "flash.h"
#include "proto.h"
#include "rf.h"
#include "fw.h"


static const uint8_t unlock_secret[PAYLOAD] = {
	#include "unlock-secret.inc"
};


static bool locked = 1;


static bool fw_payload(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	if (!seq) {
		locked = memcmp(unlock_secret, payload, PAYLOAD) != 0;
		flash_start();
		return 1;
	}
	if (locked)
		return 0;
	if (!flash_can_write(PAYLOAD))
		return 0;
	flash_write(payload, PAYLOAD);
	if (seq == limit)
		flash_end_write();
	return 1;
}


bool fw_packet(const uint8_t *buf, uint8_t len)
{
	static uint8_t seq = 0;
	static uint8_t limit;
	uint8_t ack[] = { FIRMWARE+1, buf[1], buf[2] };

	/* short (barely visible) flash to indicate reception */
	SET(LED_B7);
	CLR(LED_B7);

	/* Check packet for formal validity */

	if (len != 64+3)
		return 0;
	if (buf[0] != FIRMWARE)
		return 0;
	if (buf[1] > buf[2])
		return 0;

	/* Synchronize sequence numbers */

	if (!buf[1]) {
		seq = buf[1];
		limit = buf[2];
	} else {
		if (buf[2] != limit)
			return 0;
		if (buf[1]+1 == seq)
			goto ack;
		if (buf[1] != seq)
			return 0;
	}

	/* Process the payload */

	if (!fw_payload(buf[1], limit, buf+3))
		return 0;
	seq++;
ack:
	/* clearly visible short blink to indicate progress */
	SET(LED_B6);
	rf_send(ack, sizeof(ack));
	CLR(LED_B6);
	return 1;
}
