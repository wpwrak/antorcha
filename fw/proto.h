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
	SAMPLE		= 10,	/* start/stop sampling */
	SAMPLE_ACK	= 11,	/* start/stop sampling acknowledgement */
	SAMPLES		= 13,	/* samples */
};


#define	XA_HIGH_DEFAULT		850
#define	XA_LOW_DEFAULT		170

#define	PX_FWD_LEFT_DEFAULT	0
#define	PX_FWD_RIGHT_DEFAULT	(MAX_LINES-1)
#define	PX_BWD_LEFT_DEFAULT	0
#define	PX_BWD_RIGHT_DEFAULT	(MAX_LINES-1)

#define	TP_FWD_START_DEFAULT	70000	/* 70 ms */
#define	TP_BWD_START_DEFAULT	50000	/* 50 ms */
#define	TP_FWD_PIX_DEFAULT	1100	/* 1.1 ms */
#define	TP_BWD_PIX_DEFAULT	1100	/* 1.1 ms */

struct params {
	/* Timer ticks */

	uint16_t clkT_period;		/* Timer period */

	/* Accelerator thresholds */

	uint16_t xa_high;		/* X acceleration high threshold */
	uint16_t xa_low;		/* X acceleration low threshold */

	/* Pixel offsets (in image) */

	uint8_t	px_fwd_left;		/* first column in forward move */
	uint8_t	px_fwd_right;		/* last column in forward move */
	uint8_t	px_bwd_left;		/* last (low) column in backward move */
	uint8_t	px_bwd_right;		/* first (high) col. in backward move */

	/* Timer periods, for imaging */

	uint16_t tp_fwd_start;		/* forward image start */
	uint16_t tp_bwd_start;		/* backward image start */
	uint8_t tp_fwd_pix;		/* pixel size in forward move */
	uint8_t tp_bwd_pix;		/* pixel size in backward move */
};

#endif /* !PROTO_H */
