/*
 * tools/libant/dump.c - Antorcha raw image dump
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
#include <stdio.h>

#include "libant.h"


int dump_binary(FILE *file, const void *canvas, int width, int height)
{
	const uint8_t *p = canvas;
	int x, y, i;
	uint8_t v;

	for (x = 0; x != width; x++) {
		for (y = 0; y != height; y += 8) {
			v = 0;
			for (i = 0; i != 8; i++)
				if  (p[((y+i)*width+x) >> 3] & (1 << (x & 7)))
					v |= 1 << i;
			if (fputc(v, file) == EOF)
				return -1;
		}
	}
	return 0;
}


int dump_xbm(FILE *file, const void *canvas, int width, int height)
{
	const uint8_t *p = canvas;
	int x, y, i, n;
	uint8_t v = 0;

	if (fprintf(file, "#define foo_width %d\n", width) < 0)
		return -1;
	if (fprintf(file, "#define foo_height %d\n", height) < 0)
		return -1;
	if (fprintf(file, "static unsigned char foo_bits[] = {\n") < 0)
		return -1;
	n = 0;
	for (y = 0; y != height; y++) {
		for (x = 0; x < width; x += 8) {
			if (n)
				if (fprintf(file, "%s 0x%02x",
				    (n-1) % 12 ? "," :
				    n == 1 ? "  " : ",\n  ", v) < 0)
					return -1;
			v = 0;
			for (i = x; i != width && i != x+8; i++)
				if (p[(y*width+i) >> 3] & (1 << (i & 7)))
					v |= 1 << (i-x);
			n++;
		}
	}
	if (fprintf(file, "%s 0x%02x};\n", (n-1) % 12 ? "," : ",\n  ", v) < 0)
		return -1;
	return 0;
}


int dump_ascii(FILE *file, const void *canvas, int width, int height)
{
	const uint8_t *p = canvas;
	int x, y;

	for (y = 0; y != height; y++) {
		for (x = 0; x != width; x++)
			if (p[(y*width+x) >> 3] & (1 << (x & 7))) {
				if (fputc('#', file) == EOF)
					return -1;
			} else {
				if (fputc('.', file) == EOF)
					return -1;
			}
		if (fputc('\n', file) == EOF)
			return -1;
	}
	return 0;
}
