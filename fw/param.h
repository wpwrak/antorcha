/*
 * fw/param.h - Parameter upload
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PARAM_H
#define	PARAM_H

#include <stdint.h>

#include "sweep.h"


extern struct sweep fwd_sweep;
extern struct sweep bwd_sweep;

extern uint16_t xa_high;
extern uint16_t xa_low;
extern uint32_t fwd_start;
extern uint32_t bwd_start;

#endif /* !PARAM_H */
