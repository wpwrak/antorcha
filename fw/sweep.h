/*
 * fw/sweep.h - Image sweep
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SWEEP_H
#define	SWEEP_H

#include <stdbool.h>
#include <stdint.h>


struct sweep {
	uint32_t wait_ticks;	/* number of ticks to wait before image */
	uint16_t pixel_ticks;	/* number of ticks per pixel */
	uint8_t left;		/* leftmost line of image */
	uint8_t right;		/* rightmost line of image */
	bool forward;		/* direction of movement */
};


void image_sweep(const struct sweep *sweep);
void sweep_init(void);

#endif /* !SWEEP_H */
