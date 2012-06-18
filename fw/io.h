/*
 * fw/io.h - I/O helper macros
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef IO_H
#define	IO_H

#include <avr/io.h>


#define	LED_B6	D, 5
#define	LED_B7	D, 6
#define	LED_B8	D, 7

#define	RF_SCLK	B, 5
#define	RF_MISO	B, 4
#define	RF_MOSI	B, 3
#define	RF_nSS	B, 2	/* called nSS in schematics */
#define	RF_IRQ	B, 1
#define	RF_nRST	B, 0

#define	__SET(port, bit)	PORT##port |= 1 << (bit)
#define	__CLR(port, bit)	PORT##port &= ~(1 << (bit))
#define	__OUT(port, bit)	DDR##port |= 1 << (bit)
#define	__IN(port, bit)	DDR##port &= ~(1 << (bit))
#define	__PIN(port, bit)	((PIN##port >> (bit)) & 1)

#define	SET(...)	__SET(__VA_ARGS__)
#define	CLR(...)	__CLR(__VA_ARGS__)
#define	OUT(...)	__OUT(__VA_ARGS__)
#define	IN(...)		__IN(__VA_ARGS__)
#define	PIN(...)	__PIN(__VA_ARGS__)

#define	__SEL_BB(v)	(v)
#define	__SEL_CC(v)	(v)
#define	__SEL_DD(v)	(v)

#if 0
#define	__SEL_BC(v)	(0)
#define	__SEL_BD(v)	(0)
#define	__SEL_CB(v)	(0)
#define	__SEL_CD(v)	(0)
#define	__SEL_DB(v)	(0)
#define	__SEL_DC(v)	(0)
#endif

#define	__MASK(sel, port, bit)	__SEL_##sel##port(1 << (bit))
#define	MASK(...)	__MASK(__VA_ARGS__)

#endif /* IO_H */
