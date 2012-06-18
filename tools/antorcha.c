/*
 * tools/antorcha.c - Antorcha control utility
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <atrf.h>
#include <misctxrx.h>

#include <hash.h>
#include <proto.h>

#include "../fw/at86rf230.h"


static int verbose = 1;


static void rf_init(struct atrf_dsc *dsc, int trim, int channel)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	 atrf_reg_write(dsc, REG_XOSC_CTRL,
            (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | channel);
	atrf_reg_write(dsc, REG_IRQ_MASK, 0xff);
	flush_interrupts(dsc);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 1);
}


static void rf_send(struct atrf_dsc *dsc, void *buf, int len)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	atrf_buf_write(dsc, buf, len);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);
	wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_PLL_LOCK, 10);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
#if 1
int i;
fprintf(stderr, "\r%d:", len);
for (i = 0; i != len; i++)
	fprintf(stderr, " %02x", ((uint8_t *) buf)[i]);;
fprintf(stderr, "\n");
#endif
}


static int rf_recv(struct atrf_dsc *dsc, void *buf, int size)
{
	uint8_t irq;
	int len;

	/* Wait up to 100 ms */
	irq = wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_RX_START | IRQ_AMI, 100);
	if (!irq)
		return 0;
	len = atrf_buf_read(dsc, buf, size);
	if (len < 0)
		exit(1);
	if (len < 3) /* CRC and LQI */
		return 0;
	return atrf_reg_read(dsc, REG_PHY_RSSI) & RX_CRC_VALID ? len-3 : 0;
}


static void ping(struct atrf_dsc *dsc)
{
	uint8_t ping[] = { 0 /*PING*/, 0, 0 };
	uint8_t buf[100];
	int got, i;

	rf_send(dsc, ping, sizeof(ping));
	got = rf_recv(dsc, buf, sizeof(buf));
	printf("%d:", got);
	for (i = 0; i != got; i++)
		printf(" %02x", buf[i]);
	printf("\n");
}


static void packet(struct atrf_dsc *dsc,
    uint8_t type, uint8_t seq, uint8_t last, void *payload, int len)
{
	uint8_t tx_buf[PAYLOAD+3] = { type, seq, last };
	uint8_t rx_buf[10];
	int got;

	assert(len == PAYLOAD);
	memcpy(tx_buf+3, payload, len);
	while (1) {
		rf_send(dsc, tx_buf, sizeof(tx_buf));
		if (verbose)
			write(2, ">", 1);
		got = rf_recv(dsc, rx_buf, sizeof(rx_buf));
		if (got <= 0)
			continue;
		if (verbose)
			write(2, "\b?", 2);
		if (got < 3)
			continue;
		if (verbose)
			write(2, "\bA", 2);
		if (rx_buf[0] != type+1 || rx_buf[1] != seq)
			continue;
		if (verbose)
			write(2, "\b.", 2);
		break;
	}
}


static const uint8_t unlock_secret[] = {
	#include "unlock-secret.inc"
};


static void unlock(struct atrf_dsc *dsc)
{
	uint8_t payload[PAYLOAD];

	if (verbose)
		write(2, "unlock   ", 9);
	memset(payload, 0, PAYLOAD);
	hash_init();
	hash_merge(unlock_secret, sizeof(unlock_secret));
	hash_merge(payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
	packet(dsc, UNLOCK, 0, 3, payload, PAYLOAD);
	packet(dsc, UNLOCK, 1, 3, payload, PAYLOAD);
	hash_end();
	hash_cp(payload, PAYLOAD, 0);
	packet(dsc, UNLOCK, 2, 3, payload, PAYLOAD);
	hash_cp(payload, PAYLOAD, PAYLOAD);
	packet(dsc, UNLOCK, 3, 3, payload, PAYLOAD);
	if (verbose)
		write(2, "\n", 1);
}


static void send_firmware(struct atrf_dsc *dsc, void *buf, int len)
{
	uint8_t payload[PAYLOAD];
	uint8_t last, seq;

	if (verbose)
		write(2, "firmware ", 9);
	last = (len+63)/64;
	seq = 0;
	while (len >= PAYLOAD) {
		packet(dsc, FIRMWARE, seq++, last, buf, PAYLOAD);
		hash_merge(buf, PAYLOAD);
		buf += PAYLOAD;
		len -= PAYLOAD;
	}
	if (len) {
		memcpy(payload, buf, len);
		memset(payload+len, 0, PAYLOAD-len);
		packet(dsc, FIRMWARE, seq++, last, payload, PAYLOAD);
		hash_merge(payload, PAYLOAD);
	}
	hash_end();
	hash_cp(payload, PAYLOAD, 0);
	packet(dsc, FIRMWARE, seq, last, payload, PAYLOAD);
	if (verbose)
		write(2, "\n", 1);
}


static void firmware(struct atrf_dsc *dsc, const char *name)
{
	FILE *file;
	uint8_t fw[12*1024];	/* we have 8-12kB */
	ssize_t len;

	file = fopen(name, "r");
	if (!file) {
		perror(name);
		exit(1);
	}
	len = fread(fw, 1, sizeof(fw), file);
	if (len < 0) {
		perror(name);
		exit(1);
	}
	fclose(file);

	unlock(dsc);
	send_firmware(dsc, fw, len);
}


int main(int argc, char **argv)
{
	struct atrf_dsc *dsc;

	dsc = atrf_open(NULL);
	if (!dsc)
		return 1;
	rf_init(dsc, 8, 15);
	firmware(dsc, argv[1]);
	return 0;
}
