/*
 * fw/flash.c - Flash interface
 *
 * Written 2011, 2012 by Werner Almesberger
 * Copyright 2011, 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Adapted from ben-wpan/atusb/fw/flash.c
 */


#include <stdint.h>

#include <avr/boot.h>
//#include <avr/pgmspace.h>

#include "flash.h"


static uint32_t payload;


void flash_start(void)
{
	payload = 0;
}


int flash_can_write(uint16_t size)
{
	return payload <= BOOT_ADDR-size;
}


void flash_write(const uint8_t *buf, uint16_t size)
{
	static uint8_t last;
        const uint8_t *p;

	for (p = buf; p != buf+size; p++) {
		if (!(payload & (SPM_PAGESIZE-1))) {
			boot_page_erase(payload);
			boot_spm_busy_wait();
		}

		if (payload & 1)
			boot_page_fill(payload, last | (*p << 8));
		else
			last = *p;
		payload++;

		if (!(payload & (SPM_PAGESIZE-1))) {
			boot_page_write(payload-SPM_PAGESIZE);
			boot_spm_busy_wait();
		}
        }
}


void flash_end_write(void)
{
	if (payload & (SPM_PAGESIZE-1)) {
		boot_page_write(payload & ~(SPM_PAGESIZE-1));
		boot_spm_busy_wait();
	}
	boot_rww_enable();
}
