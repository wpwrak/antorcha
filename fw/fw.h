/*
 * fw/fw.h - Firmware upload protocol
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef FW_H
#define	FW_H

#include <stdint.h>


bool fw_packet(const uint8_t *buf, uint8_t len);

#endif /* !FW_H */
