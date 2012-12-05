/*
 * fw/mmc-hw.c - MMC hardware interface
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#ifdef AVR

#define F_CPU   8000000UL
#include <util/delay.h>

#include "io.h"

#else /* AVR */

#include <unistd.h>

#include "ben-io.h"


static inline void _delay_ms(int ms)
{
        usleep(1000*ms);
}


#endif /* !AVR */

#include "mmc-hw.h"


/*
 * Pin assignment from
 * http://alumni.cs.ucr.edu/~amitra/sdcard/ProdManualSDCardv1.9.pdf
 *
 * Pin	SD mode	SPI mode
 * 1	DAT3	Chip Select (active low)
 * 2	CMD	MOSI
 * 3	VSS	VSS
 * 4	VDD	VDD
 * 5	CLK	Clock
 * 6	VSS	VSS
 * 7	DAT0	MISO
 */


#define	MMC_nCS		CARD_DAT3
#define	MMC_MOSI	CARD_CMD
#define	MMC_CLK		CARD_CLK
#define	MMC_MISO	CARD_DAT0


void mmc_select(void)
{
	CLR(MMC_nCS);
}

void mmc_deselect(void)
{
	SET(MMC_nCS);
}


uint8_t mmc_recv(void)
{
	uint8_t i, v = 0;

	SET(MMC_MOSI);
	for (i = 0; i != 8; i++) {
		SET(MMC_CLK);
		v = (v << 1) | PIN(MMC_MISO);
		CLR(MMC_CLK);
	}
	return v;
}


void mmc_send(uint8_t v)
{
	uint8_t i;

	for (i = 0; i != 8; i++) {
		if (v & 0x80)
			SET(MMC_MOSI);
		else
			CLR(MMC_MOSI);
		SET(MMC_CLK);
		CLR(MMC_CLK);
		v <<= 1;
	}
}


void mmc_activate(void)
{
	SET(MMC_nCS);
	CLR(MMC_CLK);
	OUT(MMC_nCS);
	SET(MMC_MOSI);
	SET(MMC_CLK);	/* for pre-charging */
	OUT(MMC_MOSI);
	OUT(MMC_CLK);
	IN(MMC_MISO);

	_delay_ms(100);	/* allow card to pre-charge, to limit inrush current */

	CLR(CARD_nPWR);

	_delay_ms(10);
}


void mmc_deactivate(void)
{
	CLR(MMC_CLK);
	CLR(MMC_nCS);
	CLR(MMC_MOSI);
	CLR(MMC_MISO);

	SET(CARD_nPWR);
	_delay_ms(10);

	OUT(MMC_MISO);
}
