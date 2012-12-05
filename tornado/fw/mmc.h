/*
 * fw/mmc.h - MMC card access
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef MMC_H
#define MMC_H

#include <stdbool.h>
#include <stdint.h>


#define	MMC_BLOCK	512


bool mmc_begin_read(uint32_t addr);
uint8_t mmc_read(void);
bool mmc_end_read(void);

bool mmc_begin_write(uint32_t addr);
void mmc_write(uint8_t data);
bool mmc_end_write(void);

void mmc_off(void);
bool mmc_init(void);

#endif /* !MMC_H */
