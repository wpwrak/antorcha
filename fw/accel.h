/*
 * fw/accel.h - Acceleration sensor
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ACCEL_H
#define ACCEL_H

#include <stdbool.h>
#include <stdint.h>


extern void (*sample)(bool x, uint16_t v);


uint16_t measure_ref(bool gnd);
void accel_start(void);

#endif /* !ACCEL_H */
