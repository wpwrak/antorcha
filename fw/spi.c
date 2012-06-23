/*
 * fw/spi.c - SPI I/O
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
#include <avr/io.h>

#include "io.h"
#include "spi.h"


#define	nSS	B, 2
#define	MOSI	B, 3
#define	MISO	B, 4
#define	SCLK	B, 8


/*
 * According to simulation, signals from MCU to transceiver  should already
 * suffer severe degradation * at fosc/4 = 2 MHz due to parasitic capacitance
 * (assumed to be 10 pF) and the series resistors (10 kOhm) of the voltage
 * dividers.
 *
 * fosc/2 = 4 MHz looks horrible in simulation and does not work reliably in
 * practice either.
 */

void spi_init(void)
{
	/* SPI mode 0, MSB first, fosc/4 */
	SPCR =
	    1 << SPE |	/* enable SPI */
	    1 << MSTR;	/* master */
//	SPSR = 1 << SPI2X; /* enable for fosc/2 */
}


void spi_begin(void)
{
	CLR(nSS);
}


uint8_t spi_io(uint8_t v)
{
	SPDR = v;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}


void spi_end(void)
{
	SET(nSS);
}
