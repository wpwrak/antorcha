/*
 * fw/mmc-hw.h - MMC hardware interface
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef MMC_HW_H
#define MMC_HW_H

#include <stdint.h>


void mmc_select(void);
void mmc_deselect(void);
uint8_t mmc_recv(void);
void mmc_send(uint8_t v);
void mmc_activate(void);
void mmc_deactivate(void);

#endif /* !MMC_HW_H */
