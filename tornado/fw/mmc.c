/*
 * fw/mmc.c - MMC card access
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Heavily influenced by the MMC driver in the Mass Storage for V-USB project
 * by "lleeloo":
 * http://we.easyelectronics.ru/AVR/usb-fleshka-na-atmega8-i-v-usb-ot-idei-k-gotovomu-ustroystvu.html
 */

#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "mmc-hw.h"
#include "mmc.h"


/* Command indices */

enum {
	MMC_GO_IDLE_STATE	= 0,
	MMC_SEND_OP_COND	= 1,
	MMC_SET_BLOCKLEN	= 16,
	MMC_READ_SINGLE_BLOCK	= 17,
	MMC_WRITE_BLOCK		= 24,
};


/* Start block tokens */

enum {
	MMC_START_SINGLE_BLOCK	= 0xfe,
};


static void mmc_begin(void)
{
	mmc_select();
}


static void mmc_end(void)
{
	mmc_send(0xff);
	mmc_send(0xff);
	mmc_deselect();
	mmc_send(0xff);
}

static void mmc_command(uint8_t cmd, uint32_t arg)
{
	mmc_send(0x40 | cmd);	/* start (0)+transmission(1)+cmd */
	mmc_send(arg >> 24);
	mmc_send(arg >> 16);
	mmc_send(arg >> 8);
	mmc_send(arg);
	mmc_send(0x95);		/* crc7+end(1) */
}


static uint8_t mmc_r1(void)
{
	uint8_t v, tries = 0xff;

	do v = mmc_recv(); 
	while ((v & 0x80) && --tries);
	return v;
}


static bool mmc_wait(void)
{
	uint8_t v, tries = 0xff;

	do {
		v = mmc_recv(); 
		if (v == MMC_START_SINGLE_BLOCK)
			return 1;
		_delay_ms(1);
	}
	while (--tries);
	return 0;
}


bool mmc_begin_read(uint32_t sector)
{
	mmc_begin();
	mmc_command(MMC_READ_SINGLE_BLOCK, sector);
	if (mmc_r1()) {
		mmc_end();
		return 0;
	} else {
		return mmc_wait();
	}
}


uint8_t mmc_read(void)
{
	return mmc_recv();
}


bool mmc_end_read(void)
{
	mmc_end();
	return 1;
}


bool mmc_begin_write(uint32_t sector)
{
	mmc_begin();
	mmc_command(MMC_WRITE_BLOCK, sector);
	if (mmc_r1()) {
		mmc_end();
		return 0;
	} else {
		mmc_send(0xff);
		mmc_send(0xff);
		mmc_send(MMC_START_SINGLE_BLOCK);
		return 1;
	}
}


void mmc_write(uint8_t data)
{
	mmc_send(data);
}


bool mmc_end_write(void)
{
	mmc_send(0xff);
	mmc_send(0xff);
	mmc_recv();
	while (!mmc_recv());
	mmc_end();
	return 1;
}


void mmc_off(void)
{
	mmc_deactivate();
}


static void send_clock(void)
{
	uint8_t i;

	for (i = 0; i != 10; i++)
		mmc_send(0xff);
}


bool mmc_init(void)
{
	uint16_t tries = 0xff;
	uint8_t res;

	mmc_activate();
	send_clock();
	mmc_begin();

	do {
		mmc_command(MMC_GO_IDLE_STATE, 0);
		if (mmc_r1() == 0x01)	/* in idle state */
			break;
	}
	while (--tries);
	mmc_end();
	if (!tries)
		return 0;

	send_clock();

	tries = 0x7ff;
	do {
		mmc_select();
		mmc_command(MMC_SEND_OP_COND, 0);
		res = mmc_r1();
		mmc_deselect();
		send_clock();
	}
	while (res && --tries);

	mmc_end();

	if (res) {
		mmc_deactivate();
		return 0;
	}

	mmc_begin();
	mmc_command(MMC_SET_BLOCKLEN, MMC_BLOCK);
	mmc_r1();
	mmc_end();

	return 1;
}
