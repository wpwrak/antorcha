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

#include <stdint.h>


struct line {
	uint8_t d;		/* port D0-D7 */
	uint8_t cb;		/* port C0-C5, B6-B7 */
};


extern const struct line *image;


struct line localize_line(uint8_t p0, uint8_t p1);

#endif /* !IMAGE_H */
