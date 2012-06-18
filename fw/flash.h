/*
 * fw/flash.h - Flash interface
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef FLASH_H
#define	FLASH_H

#include <stdint.h>


void flash_start(uint32_t addr);
int flash_can_write(uint16_t size);
void flash_write(const uint8_t *buf, uint16_t size);
void flash_end_write(void);

#endif /* !FLASH_H */
