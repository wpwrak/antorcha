/*
 * tools/ant-cl/ant-cl.c - Antorcha control utility
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
#include <signal.h>
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

		fprintf(stderr, "\rSEND %d:", len);
		for (i = 0; i != len; i++)
			fprintf(stderr, " %02x", ((uint8_t *) buf)[i]);;
		fprintf(stderr, "\n");
	}
}


static int rf_recv(struct atrf_dsc *dsc, void *buf, int size, int timeout_ms)
{
	uint8_t irq;
	int len;

	/* Wait up to 100 ms */
	irq = wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_RX_START | IRQ_AMI, timeout_ms);
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


static int recv_ack(struct atrf_dsc *dsc, uint8_t *buf, int buf_len,
    uint8_t type, uint8_t seq, int timeout_ms)
{
	int got;

	if (verbose)
		write(2, ">", 1);
	got = rf_recv(dsc, buf, buf_len, timeout_ms);
	if (got <= 0) {
		if (!seq && verbose)
			write(2, "\b", 1);
		return 0;
	}

	if (debug) {
		int i;

		fprintf(stderr, "\rRECV %d:", got);
		for (i = 0; i != got; i++)
			fprintf(stderr, " %02x", buf[i]);;
		fprintf(stderr, "\n");
	}

	if (verbose)
		write(2, "\b?", 2);
	if (got < 3)
		return 0;
	if (verbose)
		write(2, "\bA", 2);
	if (buf[0] != type || buf[1] != seq)
		return 0;
	if (verbose)
		write(2, "\b.", 2);
	return got;
}


static void packet(struct atrf_dsc *dsc,
    uint8_t type, uint8_t seq, uint8_t last, const void *payload, int len)
{
	uint8_t rx_buf[10];
	int got;

	while (1) {
		packet_noack(dsc, type, seq, last, payload, len);
		got = recv_ack(dsc, rx_buf, sizeof(rx_buf), type+1, seq, 100);
		if (got)
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
	got = rf_recv(dsc, buf, sizeof(buf), 100);
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
	hash_merge(payload, PAYLOAD);
	packet(dsc, IMAGE, seq++, last, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
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


/* ----- Parameter upload -------------------------------------------------- */


static struct params params = {
	.xa_high	= XA_HIGH_DEFAULT,
	.xa_low		= XA_LOW_DEFAULT,
	.px_fwd_left	= PX_FWD_LEFT_DEFAULT,
	.px_fwd_right	= PX_FWD_RIGHT_DEFAULT,
	.px_bwd_left	= PX_BWD_LEFT_DEFAULT,
	.px_bwd_right	= PX_BWD_RIGHT_DEFAULT,
	.tp_fwd_start	= TP_FWD_START_DEFAULT,
	.tp_bwd_start	= TP_BWD_START_DEFAULT,
	.tp_fwd_pix	= TP_FWD_PIX_DEFAULT,
	.tp_bwd_pix	= TP_BWD_PIX_DEFAULT,
};

static struct map {
	const char *name;	/* NULL to mark end */
	void *p;
	int bytes;		/* 1, 2, or 4 */
} map[] = {
	{ "xa_high",		&params.xa_high,	2 },
	{ "xa_low",		&params.xa_low,		2 },
	{ "px_fwd_left",	&params.px_fwd_left,	1 },
	{ "px_bwd_left",	&params.px_bwd_left,	1 },
	{ "px_fwd_right",	&params.px_fwd_right,	1 },
	{ "px_bwd_right",	&params.px_bwd_right,	1 },
	{ "tp_fwd_start",	&params.tp_fwd_start,	4 },
	{ "tp_bwd_start",	&params.tp_bwd_start,	4 },
	{ "tp_fwd_pix",		&params.tp_fwd_pix,	2 },
	{ "tp_bwd_pix",		&params.tp_bwd_pix,	2 },
	{ NULL }
};

static int have_params = 0;


static void add_param(const char *arg, const char *eq)
{
	const struct map *m;
	size_t len;
	unsigned long v;
	char *end;
	uint8_t *p;
	int i;

	len = eq-arg;
	for (m = map; m->name; m++) {
		if (strlen(m->name) != len)
			continue;
		if (!strncmp(arg, m->name, len))
			break;
	}
	if (!m->name) {
		fprintf(stderr, "unknown parameter \"%.*s\"\n",
		    (int) len, arg);
		exit(1);
	}
	v = strtoul(eq+1, &end, 10);
	if (*end) {
		fprintf(stderr, "invalid value \"%s\"\n", eq+1);
		exit(1);
	}
	p = m->p;
	for (i = 0; i != m->bytes; i++) {
		*p++ = v;
		v >>= 8;
	}
	if (v) {
		fprintf(stderr, "value \"%s\" is too large\n", eq+1);
		exit(1);
	}
	have_params = 1;
}


static void send_param(struct atrf_dsc *dsc)
{
	uint8_t payload[PAYLOAD];

	if (!have_params)
		return;
	hash_init();
	hash_merge(image_secret, sizeof(image_secret));
	if (verbose)
		write(2, "param ", 6);
	memset(payload, 0, PAYLOAD);
	memcpy(payload, &params, sizeof(params));
	packet(dsc, PARAM, 0, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);

	/* @@@ salt */
	packet(dsc, PARAM, 1, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
	packet(dsc, PARAM, 2, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
	hash_end();

	/* hash */
	hash_cp(payload, PAYLOAD, 0);
	packet(dsc, PARAM, 3, 4, payload, PAYLOAD);
	hash_cp(payload, PAYLOAD, PAYLOAD);
	packet(dsc, PARAM, 4, 4, payload, PAYLOAD);

	if (verbose)
		write(2, "\n", 1);
}


/* ----- Reset ------------------------------------------------------------- */


static void reset(struct atrf_dsc *dsc)
{
	packet_noack(dsc, RESET, 0, 0, unlock_secret, PAYLOAD);
}


/* ----- Samples ----------------------------------------------------------- */


static volatile int run = 1;


static void sigint(int sig)
{
	run = 0;
}


static int read_sample(uint8_t **s, uint16_t *t_high, uint16_t *t_low,
    uint16_t *last)
{
	int v;

	*t_low = *(*s)++;
	*t_low |= *(*s)++ << 8;
	if (*t_low < *last)
		(*t_high)++;
	*last = *t_low;
	v = *(*s)++;
	v |= *(*s)++ << 8;
	return v;
}


static void samples(struct atrf_dsc *dsc, int gui)
{
	uint8_t buf[MAX_PSDU] = { 0, };
	int got;
	uint8_t *s;
	uint16_t t_high, t_low, last;
	int x, y;
	double t;

	buf[0] = 1;
	packet(dsc, SAMPLE, 0, 0, buf, PAYLOAD);

#ifdef GFX
	if (gui)
		plot_init();
	else
#endif
		signal(SIGINT, sigint);
	while (run) {
		got = rf_recv(dsc, buf, sizeof(buf), 100);
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
		s = buf+3;
		t_high = *s++;
		t_high |= *s++ << 8;
		last = 0;
		while (s < buf+got-2) {
			x = read_sample(&s, &t_high, &t_low, &last);
			t = (t_high << 16 | t_low)/1000000.0;
			if (debug)
				fprintf(stderr, "\t%11.6f %d", t, x);
			if (!gui)
				printf("%11.6f\t%d\t", t, x);

			y = read_sample(&s, &t_high, &t_low, &last);
			t = (t_high << 16 | t_low)/1000000.0;
			if (debug)
				fprintf(stderr, "\t%11.6f %d\n", t, y);
			if (!gui)
				printf("%11.6f\t%d\n", t, y);

#ifdef GFX
			if (gui && !plot(x, y))
				goto quit;
#endif
		}
	}
#ifdef GFX
quit:
#endif
	buf[0] = 0;
	packet(dsc, SAMPLE, 0, 0, buf, PAYLOAD);
}


/* ----- Diagnostics ------------------------------------------------------- */


static void send_diag(struct atrf_dsc *dsc, uint16_t pattern,
    uint16_t *gnd, uint16_t *v_bg)
{
	uint8_t payload[PAYLOAD];
	int got, i;

	hash_init();
	hash_merge(image_secret, sizeof(image_secret));
	if (verbose)
		write(2, "diag ", 5);
	memset(payload, 0, PAYLOAD);
	payload[0] = pattern;
	payload[1] = pattern >> 8;
	packet(dsc, DIAG, 0, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);

	/* @@@ salt */
	packet(dsc, DIAG, 1, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
	packet(dsc, DIAG, 2, 4, payload, PAYLOAD);
	hash_merge(payload, PAYLOAD);
	hash_end();

	/* hash */
	hash_cp(payload, PAYLOAD, 0);
	packet(dsc, DIAG, 3, 4, payload, PAYLOAD);
	hash_cp(payload, PAYLOAD, PAYLOAD);

	while (1) {
		packet_noack(dsc, DIAG, 4, 4, payload, PAYLOAD);
		if (verbose)
			write(2, ">", 1);
		got = recv_ack(dsc, payload, PAYLOAD, DIAG_ACK, 4, 1000);
		if (got >= 7)
			break;
	}
	if (verbose)
		write(2, "\n", 1);
	for (i = 0; i != DIAG_SAMPLES; i++) {
		gnd[i] = payload[3+4*i] | payload[4+4*i] << 8;
		v_bg[i] = payload[5+4*i] | payload[6+4*i] << 8;
	}
}


static void diag_1(struct atrf_dsc *dsc, uint16_t pattern)
{
	uint16_t gnd[DIAG_SAMPLES], v_bg[DIAG_SAMPLES];
	int i;

	send_diag(dsc, pattern, gnd, v_bg);
	printf("%d", pattern);
	for (i = 0; i != DIAG_SAMPLES; i++)
		printf(" %f %f", 1.1*1024/gnd[i], 1.1*1024/v_bg[i]);
	printf("\n");
	fflush(stdout);
}


static void diag(struct atrf_dsc *dsc)
{
	int i;

	diag_1(dsc, 0);
	for (i = 0; i != 16; i++)
		diag_1(dsc, 1 << i);
	for (i = 0; i <= 16; i++)
		diag_1(dsc, (1 << i)-1);
}


/* ----- Command-line processing ------------------------------------------- */


static unsigned get_int(uint8_t *p, int n)
{
	unsigned v = 0;
	int i;

	for (i = 0; i != n; i++)
		v |= *p++ << (i*8);
	return v;
}


static void usage(const char *name)
{
	const struct map *m;

	fprintf(stderr,
"usage: %s [-d] [param=value ...] image_file\n"
   "%6s %s [-d] -D\n"
   "%6s %s [-d] -F firmware_file\n"
   "%6s %s [-d] -P\n"
   "%6s %s [-d] -R\n"
   "%6s %s [-d] -S [-S]\n\n"
"  -D       run diagnostics\n"
"  -F file  firmware upload\n"
"  -P       ping (version query)\n"
"  -R       reset\n"
"  -S       sample with output on stdout. Exit with ^C.\n"
#ifdef GFX
"  -S -S    sample with graphical output. Exit with Q.\n\n"
#endif
"Parameters:\n"
    , name, "", name, "", name, "", name, "", name, "", name);
	for (m = map; m->name; m++)
		fprintf(stderr, "  %s\t(default %u)\n",
		    m->name, get_int(m->p, m->bytes));
	exit(1);
}


int main(int argc, char **argv)
{
	const char *fw = NULL;
	int do_ping = 0, do_reset = 0, do_sample = 0, do_diag = 0;
	struct atrf_dsc *dsc;
	int c;

	while ((c = getopt(argc, argv, "dDF:PRS")) != EOF)
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'D':
			do_diag = 1;
			break;
		case 'F':
			fw = optarg;
			break;
		case 'P':
			do_ping = 1;
			break;
		case 'R':
			do_reset = 1;
			break;
		case 'S':
			do_sample++;
			break;
		default:
			usage(*argv);
		}

	if (do_ping+do_reset+do_diag+!!do_sample+!!fw > 1)
		usage(*argv);
	if (do_ping || do_reset || do_diag || do_sample || fw) {
		if (argc != optind)
			usage(*argv);
	} else {
		if (argc == optind)
			usage(*argv);
	}

	dsc = atrf_open(NULL);
	if (!dsc)
		return 1;

	rf_init(dsc, 8, 15);
	if (do_ping)
		ping(dsc);
	else if (do_reset)
		reset(dsc);
	else if (do_sample)
		samples(dsc, do_sample > 1);
	else if (do_diag)
		diag(dsc);
	else if (fw)
		firmware(dsc, fw);
	else {
		const char *eq;

		while (optind != argc) {
			eq = strchr(argv[optind], '=');
			if (!eq)
				break;
			add_param(argv[optind], eq);
			optind++;
		}
		if (argc > optind+1)
			usage(*argv);
		if (optind != argc)
			image(dsc, argv[optind]);
		send_param(dsc);
	}

	return 0;
}
