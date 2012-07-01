/*
 * fw/secret.c - Shared authentication secrets
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include <avr/pgmspace.h>

#include "proto.h"
#include "secret.h"


const uint8_t maint_secret[PAYLOAD] PROGMEM = {
	#include "unlock-secret.inc"
};


const uint8_t image_secret[2*PAYLOAD] PROGMEM = {
        #include "image-secret.inc"
};
