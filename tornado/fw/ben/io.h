/*
 * ben/io.h - I/O helper macros (for cross-platform testing on the Ben)
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BEN_IO_H
#define	BEN_IO_H

#include <stdint.h>


#define	CARD_nPWR	(1 << 2)	/* PD02 */
#define	CARD_DAT2	(1 << 12)	/* PD12 */
#define	CARD_DAT3	(1 << 13)	/* PD13 */
#define	CARD_CMD	(1 << 8)	/* PD08 */
#define	CARD_CLK	(1 << 9)	/* PD09 */
#define	CARD_DAT0	(1 << 10)	/* PD10 */
#define	CARD_DAT1	(1 << 11)	/* PD11 */


#define	REG(n)	(*(volatile uint32_t *) (ben_mem+(n)))

#define GPIO(n)	REG(0x10000+(n))

#define PDPIN	GPIO(0x300)	/* port D pin level */
#define PDDATS	GPIO(0x314)	/* port D data set */
#define PDDATC	GPIO(0x318)	/* port D data clear */
#define PDFUNS	GPIO(0x344)	/* port D function set */
#define PDFUNC	GPIO(0x348)	/* port D function clear */
#define PDDIRS	GPIO(0x364)	/* port D direction set */
#define PDDIRC	GPIO(0x368)	/* port D direction clear */

#define	SET(mask)	PDDATS = mask
#define	CLR(mask)	PDDATC = mask
#define	OUT(mask)	PDDIRS = mask
#define	IN(mask)	PDDIRC = mask
#define	PIN(mask)	(!!(PDPIN & (mask)))


extern void *ben_mem;

#endif /* BEN_IO_H */
