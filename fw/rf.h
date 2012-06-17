/*
 * fw/rf.h - RF interface
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RF_H
#define	RF_H

#include <stdint.h>


void rf_init(void);
void rf_send(const void *buf, uint8_t size);
uint8_t rf_recv(void *buf, uint8_t size);

#endif /* !RF_H */
