/*
 * fw/image.h - Image data (upload and conversion)
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef IMAGE_H
#define	IMAGE_H

#include <stdbool.h>
#include <stdint.h>


#define	MAX_LINES	100


struct line {
	uint8_t d;		/* port D0-D7 */
	uint8_t cb;		/* port C0-C5, B6-B7 */
};


extern struct line image[MAX_LINES];

#endif /* !IMAGE_H */