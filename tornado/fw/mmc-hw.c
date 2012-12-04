/*
 * fw/mmc-hw.c - MMC hardware interface
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include "io.h"
#include "mmc-hw.h"


void mmc_select(void)
{
//	CS = 0;
}

void mmc_deselect(void)
{
//	CS = 1;
}


uint8_t mmc_recv(void)
{
//	MSB first
	return 0;
}


void mmc_send(uint8_t v)
{
//	MSB first
}


void mmc_activate(void)
{
//	set pull-ups
//	VDD = 1;
}


void mmc_deactivate(void)
{
//	set directions
//	VDD = 0;
}
