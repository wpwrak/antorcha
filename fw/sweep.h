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
	uint32_t start_ticks;	/* absolute start time */
	uint16_t pixel_ticks;	/* number of ticks per pixel */
	uint8_t left;		/* leftmost line of image */
	uint8_t right;		/* rightmost line of image */
	bool forward;		/* direction of movement */
};


extern volatile bool sweeping;

uint32_t uptime_irq(void);
uint32_t uptime(void);

void sweep_image(const struct sweep *sweep);
void sweep_init(void);

#endif /* !SWEEP_H */
