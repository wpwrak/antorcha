/*
 * fw/secret.h - Shared authentication secrets
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SECRET_H
#define	SECRET_H

#include <stdint.h>

#include <avr/pgmspace.h>

#include "proto.h"


extern const uint8_t maint_secret[PAYLOAD] PROGMEM;
extern const uint8_t image_secret[2*PAYLOAD] PROGMEM;

#endif /* !SECRET_H */
