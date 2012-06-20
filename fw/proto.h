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

#include <stdint.h>


#define	PAYLOAD		64	/* most messages use a fixed 64 bytes payload */

enum pck_type {
	PING		= 0,	/* version query */
	PONG		= 1,	/* version response */
	RESET		= 2,	/* reset to boot loader */
	FIRMWARE	= 4,	/* firmware upload */
	FIRMWARE_ACK	= 5,	/* firmware upload acknowledgement */
	IMAGE		= 6,	/* image upload */
	IMAGE_ACK	= 7,	/* image upload acknowledgement */
	PARAM		= 8,	/* parameter upload */
	PARAM_ACK	= 9,	/* parameter upload acknowledgement */
};


struct params {
	/* Timer ticks */

	uint16_t clkT_period;		/* Timer period */

	/* Accelerator thresholds */

	uint16_t xa_high;		/* X acceleration high threshold */
	uint16_t xa_low;		/* X acceleration low threshold */

	/* Pixel offsets (in image) */

	uint8_t	px_fwd_img_first;	/* first column in forward move */
	uint8_t	px_fwd_img_end;		/* last column in forward move */
	uint8_t	px_bwd_img_first;	/* first (high) col. in backward move */
	uint8_t	px_bwd_img_end;		/* last (low) column in backward move */

	/* Timer periods, for imaging */

	uint16_t tp_fwd_start;		/* forward image start */
	uint16_t tp_bwd_start;		/* backward image start */
	uint8_t tp_fwd_pix;		/* pixel size in forward move */
	uint8_t tp_bwd_pix;		/* pixel size in backward move */

	/* Timer periods, for estimation */

	uint16_t tp_fwd;		/* forward half-period */
	uint16_t tp_bwd;		/* backward half-period */
};

#endif /* !PROTO_H */
