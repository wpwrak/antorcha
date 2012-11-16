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


#define	LED_DS		D, 1
#define	LED_LCLK	D, 2
#define	LED_SCLK	D, 4

#define	CARD_nPWR	B, 6
#define	CARD_DAT2	B, 2
#define	CARD_DAT3	B, 1
#define	CARD_CMD	B, 3
#define	CARD_CLK	B, 0
#define	CARD_DAT0	B, 4
#define	CARD_DAT1	B, 5

#define	SW_N		C, 3
#define	SW_E		C, 4
#define	SW_S		C, 0
#define	SW_W		C, 2
#define	SW_SW		C, 1

#define	ADC_X		6
#define	ADC_Y		7

#define	__SET(port, bit)	PORT##port |= 1 << (bit)
#define	__CLR(port, bit)	PORT##port &= ~(1 << (bit))
#define	__OUT(port, bit)	DDR##port |= 1 << (bit)
#define	__IN(port, bit)		DDR##port &= ~(1 << (bit))
#define	__PIN(port, bit)	((PIN##port >> (bit)) & 1)

#define	SET(...)	__SET(__VA_ARGS__)
#define	CLR(...)	__CLR(__VA_ARGS__)
#define	OUT(...)	__OUT(__VA_ARGS__)
#define	IN(...)		__IN(__VA_ARGS__)
#define	PIN(...)	__PIN(__VA_ARGS__)

#define	__SEL_BB(v)	(v)
#define	__SEL_CC(v)	(v)
#define	__SEL_DD(v)	(v)

#define	__SEL_BC(v)	(0)
#define	__SEL_BD(v)	(0)
#define	__SEL_CB(v)	(0)
#define	__SEL_CD(v)	(0)
#define	__SEL_DB(v)	(0)
#define	__SEL_DC(v)	(0)

#define	__MASK(sel, port, bit)	__SEL_##sel##port(1 << (bit))
#define	MASK(...)	__MASK(__VA_ARGS__)

#endif /* IO_H */
