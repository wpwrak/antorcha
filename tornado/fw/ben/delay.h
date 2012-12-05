/*
 * fw/ben/delay.h - Ben-specific access to _delay_ms
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BEN_DELAY_H
#define	BEN_DELAY_H

#include <unistd.h>


static inline void _delay_ms(int ms)
{
	usleep(1000*ms);
}

#endif /* BEN_DELAY_H */
