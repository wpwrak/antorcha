/*
 * fw/dispatch.h - Wireless protocol dispatcher
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef DISPATCH_H
#define	DISPATCH_H

#include <stdbool.h>
#include <stdint.h>

#include "proto.h"


struct handler {
	enum pck_type type;
	bool (*first)(uint8_t limit, const uint8_t *payload);
	bool (*more)(uint8_t seq, uint8_t limit, const uint8_t *payload);
};


extern struct handler image_handler;
extern struct handler reset_handler;
extern struct handler sample_handler;
extern struct handler param_handler;
extern struct handler diag_handler;


bool dispatch(const uint8_t *buf, uint8_t len, const struct handler **protos);

#endif /* !PROTO_H */
