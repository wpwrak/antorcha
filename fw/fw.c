/*
 * fw/fw.h - Firmware upload protocols
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

#include "hash.h"
#include "flash.h"
#include "proto.h"
#include "dispatch.h"
#include "fw.h"


static const uint8_t unlock_secret[] = {
	#include "unlock-secret.inc"
};


static void panic(void)
{
	/* ??? */
}


/* ----- Unlocking --------------------------------------------------------- */


static bool unlocked = 0;
static bool unlock_failed;


static bool unlock_first(const uint8_t *payload)
{
	hash_init();
	hash_merge(unlock_secret, sizeof(unlock_secret));
	hash_merge(payload, PAYLOAD);
	unlocked = 0;
	unlock_failed = 0;
	return 1;
}


static bool unlock_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	switch (seq) {
	case 1:
		hash_merge(payload, PAYLOAD);
		hash_end();
		break;
	case 2:
		if (!hash_eq(payload, PAYLOAD, 0))
			unlock_failed = 1;
		break;
	case 3:
		if (unlock_failed)
			return 1;
		if (hash_eq(payload, PAYLOAD, PAYLOAD))
			unlocked = 1;
		else
			unlock_failed = 1;
		break;
	default:
		return 0;
	}
	return 1;
}


static const struct handler unlock_proto = {
	.type	= UNLOCK,
	.first	= unlock_first,
	.more	= unlock_more,
};


/* ----- Firmware upload --------------------------------------------------- */


static bool fw_first(const uint8_t *payload)
{
//	if (!unlocked)
//		return 0;
	hash_init();
	hash_merge(payload, PAYLOAD);
	flash_start(APP_ADDR);
	flash_write(payload, PAYLOAD);
	return 1;
}


static bool fw_more(uint8_t seq, uint8_t limit, const uint8_t *payload)
{
	if (!flash_can_write(PAYLOAD))
		return 0;
	if (seq != limit) {
		hash_merge(payload, PAYLOAD);
		flash_write(payload, PAYLOAD);
		return 1;
	}
	flash_end_write();
	hash_end();
	if (!hash_eq(payload, PAYLOAD, 0))
		panic();
	return 1;
}


static const struct handler fw_proto = {
	.type	= FIRMWARE,
	.first	= fw_first,
	.more	= fw_more,
};


/* ----- Protocol table ---------------------------------------------------- */


const struct handler *fw_protos[] = {
	&unlock_proto,
	&fw_proto,
	NULL
};
