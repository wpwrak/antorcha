/*
 * fw/reset.c - Reset protocol
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

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "proto.h"
#include "dispatch.h"


/*
 * For now, we use the unlock secret for reset as well. The reasoning is as
 * follows:
 *
 * - both secrets are transmitted in the clear and can therefore only be
 *   used in a trusted environment,
 * - both secrets are used mainly during development and not needed for regular
 *   operation,
 * - RESET is normally followed immediately by FIRMWARE, so a listener could
 *   intercept the secret(s) either way.
 */

static const uint8_t reset_secret[PAYLOAD] PROGMEM = {
	#include "unlock-secret.inc"
};


static bool reset_first(uint8_t limit, const uint8_t *payload)
{
	if (memcmp_P(payload, reset_secret, PAYLOAD))
		return 0;
	WDTCSR = 1 << WDE;
	return 1;
}


struct handler reset_handler = {
	.type	= RESET,
	.first	= reset_first,
};
