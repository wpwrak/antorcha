/*
 * fw/proto.h - Wireless protocol constants
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef PROTO_H
#define	PROTO_H

#define	PAYLOAD		64	/* most messages use a fixed 64 bytes payload */

enum pck_type {
	PING		= 0,	/* version query */
	PONG		= 1,	/* version response */
	UNLOCK		= 2,	/* unlock firmware upload */
	UNLOCK_ACK	= 3,	/* unlock acknowledgement */
	FIRMWARE	= 4,	/* firmware upload */
	FIRMWARE_ACK	= 5,	/* firmware upload acknowledgement */
	IMAGE		= 6,	/* image upload */
	IMAGE_ACK	= 7,	/* image upload acknowledgement */
	PARAM		= 8,	/* parameter upload */
	PARAM_ACK	= 9,	/* parameter upload acknowledgement */
};

#endif /* !PROTO_H */
