/*
 * fw/rf.c - RF interface
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
#define F_CPU	8000000UL
#include <util/delay.h>

#include "at86rf230.h"
#include "io.h"
#include "spi.h"
#include "rf.h"


/*
 * According to IEEE 802.15.4-2003 section E.2.6, channel 15 is the only
 * channel that falls into the 802.11 guard bands in North America an Europe.
 */

#define	DEFAULT_CHAN	15	/* channel 15, 2425 MHz */

/*
 * Transmit power, dBm. IEEE 802.15.4-2003 section E.3.1.3 specifies a transmit
 * power of 0 dBm for IEEE 802.15.4. We assume an antenna gain of 3 dB or
 * better.
 */

#define	DEFAULT_POWER	-3.2	/* transmit power, dBm */


static uint8_t reg_read(uint8_t reg)
{
	uint8_t value;

	spi_begin();
	spi_io(AT86RF230_REG_READ | reg);
	value = spi_io(0);
	spi_end();

	return value;
}


static void reg_write(uint8_t reg, uint8_t value)
{
	spi_begin();
	spi_io(AT86RF230_REG_WRITE | reg);
	spi_io(value);
	spi_end();
}

void rf_init(void)
{
	spi_init();
	CLR(RF_nRST);
	_delay_us(10);	/* tTR10 = 625 ns */
	SET(RF_nRST);
	_delay_us(40);	/* tTR13 = 37 us */

	reg_write(REG_TRX_STATE, TRX_CMD_TRX_OFF);
	_delay_us(40);	/* tTR13 = 37 us */
	reg_write(REG_PHY_CC_CCA, DEFAULT_CHAN);

	reg_write(REG_TRX_STATE, TRX_CMD_RX_ON);
	_delay_us(200);	/* tTR19(max) = 166 us */

	//reg_write(REG_IRQ_MASK, IRQ_TRX_END);
	reg_write(REG_IRQ_MASK, 0xff);
	reg_read(REG_IRQ_STATUS);
}


void rf_send(const void *buf, uint8_t size)
{
	uint8_t i;

	reg_write(REG_TRX_STATE, TRX_CMD_PLL_ON);
	_delay_us(1);	/* tTR9 = 1 us */

	spi_begin();
	spi_io(AT86RF230_BUF_WRITE);
	spi_io(size+2); /* CRC */
	for (i = 0; i != size; i++)
		spi_io(((const uint8_t *) buf)[i]);
	spi_end();

	reg_read(REG_IRQ_STATUS);

	reg_write(REG_TRX_STATE, TRX_CMD_TX_START);
	_delay_us(16);	/* tTR10 = 16 us */

	while (!(reg_read(REG_IRQ_STATUS) & IRQ_TRX_END)) {
		PORTC = 1;
		PORTC = 0;
	}

	reg_write(REG_TRX_STATE, TRX_CMD_RX_ON);
	_delay_us(1);	/* tTR8 = 1 us */
}


uint8_t rf_recv(void *buf, uint8_t size)
{
	uint8_t irq, len, i;

	irq = reg_read(REG_IRQ_STATUS);
	if (!(irq & IRQ_TRX_END))
		return 0;

	if (!(reg_read(REG_PHY_RSSI) & RX_CRC_VALID))
		return 0;

	spi_begin();
	spi_io(AT86RF230_BUF_READ);
	len = spi_io(0);
	if (!len || (len & 0x80)) {
		spi_end();
		return 0;
	}
	if (size > len)
		size = len;
	for (i = 0; i != size; i++)
		((uint8_t *) buf)[i] = spi_io(0);
	spi_end();
	return len;
}
