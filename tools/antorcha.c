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

#include <at86rf230.h>
#include <atrf.h>
#include <misctxrx.h>

#include <proto.h>

#include "hash.h"
#include "plot.h"


static int verbose = 1;
static int debug = 0;


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
	uint8_t tmp[MAX_PSDU];

	/* Copy the message to append the CRC placeholders */
	memcpy(tmp, buf, len);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	flush_interrupts(dsc);
	atrf_buf_write(dsc, tmp, len+2);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);
	wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_PLL_LOCK, 10);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);

	if (debug) {
		int i;
		fprintf(stderr, "\r%d:", len);
		for (i = 0; i != len; i++)
			fprintf(stderr, " %02x", ((uint8_t *) buf)[i]);;
		fprintf(stderr, "\n");
	}
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


static void packet_noack(struct atrf_dsc *dsc,
    uint8_t type, uint8_t seq, uint8_t last, const void *payload, int len)
{
	uint8_t tx_buf[PAYLOAD+3] = { type, seq, last };

	assert(len == PAYLOAD);
	memcpy(tx_buf+3, payload, len);
	rf_send(dsc, tx_buf, sizeof(tx_buf));
}


static void packet(struct atrf_dsc *dsc,
    uint8_t type, uint8_t seq, uint8_t last, const void *payload, int len)
{
	uint8_t rx_buf[10];
	int got;

	while (1) {
		packet_noack(dsc, type, seq, last, payload, len);
		if (verbose)
			write(2, ">", 1);
		got = rf_recv(dsc, rx_buf, sizeof(rx_buf));
		if (got <= 0) {
			if (!seq && verbose)
				write(2, "\b", 1);
			continue;
		}
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


/* ----- Ping -------------------------------------------------------------- */


static void ping(struct atrf_dsc *dsc)
{
	uint8_t ping[] = { PING, 0, 0 };
	uint8_t buf[100];
	int got, i;

	rf_send(dsc, ping, sizeof(ping));
	got = rf_recv(dsc, buf, sizeof(buf));
	if (!got)
		return;
	printf("%d: ", got);
	for (i = 3; i != got; i++)
		printf("%c", buf[i] >= ' ' && buf[i] <= '~' ? buf[i] : '?');
	printf("\n");
}


/* ----- Firmware upload --------------------------------------------------- */


static const uint8_t unlock_secret[PAYLOAD] = {
	#include "unlock-secret.inc"
};


static void send_firmware(struct atrf_dsc *dsc, void *buf, int len)
{
	uint8_t payload[PAYLOAD];
	uint8_t last, seq;

	if (verbose)
		write(2, "firmware ", 9);
	last = (len+63)/64;
	seq = 0;
	packet_noack(dsc, RESET, 0, 0, unlock_secret, PAYLOAD);
	packet(dsc, FIRMWARE, seq++, last, unlock_secret, PAYLOAD);
	while (len >= PAYLOAD) {
		packet(dsc, FIRMWARE, seq++, last, buf, PAYLOAD);
		buf += PAYLOAD;
		len -= PAYLOAD;
	}
	if (len) {
		memcpy(payload, buf, len);
		memset(payload+len, 0, PAYLOAD-len);
		packet(dsc, FIRMWARE, seq++, last, payload, PAYLOAD);
	}
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

	send_firmware(dsc, fw, len);
}


/* ----- Image upload ------------------------------------------------------ */


static const uint8_t image_secret[PAYLOAD*2] = {
	#include "image-secret.inc"
};


static void send_image(struct atrf_dsc *dsc, void *buf, int len)
{
	uint8_t payload[PAYLOAD];
	uint8_t last, seq;

	hash_init();
	hash_merge(image_secret, sizeof(image_secret));
	if (verbose)
		write(2, "image ", 6);

	last = (len+63)/64+3;
	seq = 0;
	while (len >= PAYLOAD) {
		packet(dsc, IMAGE, seq++, last, buf, PAYLOAD);
		hash_merge(buf, PAYLOAD);
		buf += PAYLOAD;
		len -= PAYLOAD;
	}
	if (len) {
		memcpy(payload, buf, len);
		memset(payload+len, 0, PAYLOAD-len);
		packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);
		hash_merge(payload, PAYLOAD);
	}

	/* @@@ salt */
	packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);
	packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);
	hash_end();

	/* hash */
	hash_cp(payload, PAYLOAD, 0);
	packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);
	hash_cp(payload, PAYLOAD, PAYLOAD);
	packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);

	if (verbose)
		write(2, "\n", 1);
}


static void image(struct atrf_dsc *dsc, const char *name)
{
	FILE *file;
	uint8_t img[200] = { 0 };
	ssize_t len;

	file = fopen(name, "r");
	if (!file) {
		perror(name);
		exit(1);
	}
	len = fread(img, 1, sizeof(img), file);
	if (len < 0) {
		perror(name);
		exit(1);
	}
	fclose(file);

	send_image(dsc, img, len);
}


/* ----- Samples ----------------------------------------------------------- */


static void samples(struct atrf_dsc *dsc)
{
	uint8_t buf[MAX_PSDU] = { 0, };
	int got;
	uint8_t *s;
	int x, y;

	buf[0] = 1;
	packet(dsc, SAMPLE, 0, 0, buf, PAYLOAD);
	plot_init();
	while (1) {
		got = rf_recv(dsc, buf, sizeof(buf));
		if (got <= 3)
			continue;
		if (buf[0] != SAMPLES)
			continue;
		if (debug > 1) {
			int i;

			for (i = 0; i != got; i++)
				fprintf(stderr, " %02x", buf[i]);
			fprintf(stderr, "\n");
		}
		if (debug)
			fprintf(stderr, "%d:", got);
		s = buf+3+2;
		while (s < buf+got-2) {
			s += 2;
			x = *s++;
			x |= *s++ << 8;
			s += 2;
			y = *s++;
			y |= *s++ << 8;
			if (debug)
				fprintf(stderr, "\t%d %d\n", x, y);
			if (!plot(x, y))
				goto quit;
		}
	}
quit:
	buf[0] = 0;
	packet(dsc, SAMPLE, 0, 0, buf, PAYLOAD);
}


/* ----- Command-line processing ------------------------------------------- */


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-d] image_file\n"
   "%6s %s [-d] -F firmware_file\n"
   "%6s %s [-d] -P\n"
   "%6s %s [-d] -S\n"
    , name, "", name, "", name, "", name);
	exit(1);
}


int main(int argc, char **argv)
{
	const char *fw = NULL;
	int do_ping = 0, do_sample = 0;
	struct atrf_dsc *dsc;
	int c;

	while ((c = getopt(argc, argv, "dF:PS")) != EOF)
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'F':
			fw = optarg;
			break;
		case 'P':
			do_ping = 1;
			break;
		case 'S':
			do_sample = 1;
			break;
		default:
			usage(*argv);
		}

	if (do_ping+do_sample+!!fw > 1)
		usage(*argv);
	if (do_ping || do_sample || fw) {
		if (argc != optind)
			usage(*argv);
	} else {
		if (argc != optind+1)
			usage(*argv);
	}

	dsc = atrf_open(NULL);
	if (!dsc)
		return 1;

	rf_init(dsc, 8, 15);
	if (do_ping)
		ping(dsc);
	else if (do_sample)
		samples(dsc);
	else if (fw)
		firmware(dsc, fw);
	else
		image(dsc, argv[optind]);

	return 0;
}
