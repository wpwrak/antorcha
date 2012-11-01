/*
 * tools/ant-txt/ant-txt.c - Antorcha text composer
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
#include <assert.h>
#include <sys/types.h>

#include <libant/libant.h>


#define	W	80
#define	H	16


static void dump_binary(const uint8_t *canvas)
{
	int x, y, i;
	uint8_t v;
	ssize_t wrote;

	for (x = 0; x != W; x++) {
		for (y = 0; y != H; y += 8) {
			v = 0;
			for (i = 0; i != 8; i++)
				if  (canvas[((y+i)*W+x) >> 3] & (1 << (x & 7)))
					v |= 1 << i;
			wrote = write(1, &v, 1);
			if (wrote < 0) {
				perror("fwrite");
				exit(1);
			}
			if (!wrote) {
				fprintf(stderr, "short write\n");
				exit(1);
			}
		}
	}
}


static void dump_xbm(const uint8_t *canvas)
{
	int x, y, i, n;
	uint8_t v = 0;

	printf("#define foo_width %d\n", W);
	printf("#define foo_height %d\n", H);
	printf("static unsigned char foo_bits[] = {\n");
	n = 0;
	for (y = 0; y != H; y++) {
		for (x = 0; x < W; x += 8) {
			if (n)
				printf("%s 0x%02x",
				    (n-1) % 12 ? "," :
				    n == 1 ? "  " : ",\n  ", v);
			v = 0;
			for (i = x; i != W && i != x+8; i++)
				if (canvas[(y*W+i) >> 3] & (1 << (i & 7)))
					v |= 1 << (i-x);
			n++;
		}
	}
	printf("%s 0x%02x};\n", (n-1) % 12 ? "," : ",\n  ", v);
}


static void dump_text(const uint8_t *canvas)
{
	int x, y;

	for (y = 0; y != H; y++) {
		for (x = 0; x != W; x++)
			if (canvas[(y*W+x) >> 3] & (1 << (x & 7)))
				putchar('#');
			else
				putchar('.');
		putchar('\n');
	}
}


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-b|-x] [-F font_dir ...] [text]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	struct edit *edits = NULL, **last = &edits;
	uint8_t *canvas;
	const char *err;
	int binary = 0, xbm = 0;
	int i, c;

	while ((c = getopt(argc, argv, "bF:x")) != EOF)
		switch (c) {
		case 'b':
			binary = 1;
			break;
		case 'x':
			xbm = 1;
			break;
		case 'F':
			add_font_dir(optarg);
			break;
		default:
			usage(*argv);
		}
	if (binary && xbm)
		usage(*argv);
	for (i = optind; i != argc; i++) {
		while (*last)
			last = &(*last)->next;
		if (edits) {
			*last = malloc(sizeof(struct edit));
			if (!*last)
				abort();
			(*last)->type = edit_nl;
			last = &(*last)->next;
		}
		*last = text2edit(argv[i], &err);
		if (!*last) {
			fprintf(stderr, "\"%s\": %s\n", argv[i], err);
			return 1;
		}
	}

	canvas = apply_edits(W, H, edits, &err);
	if (!canvas) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	if (binary)
		dump_binary(canvas);
	else if (xbm)
		dump_xbm(canvas);
	else
		dump_text(canvas);
	return 0;
}
