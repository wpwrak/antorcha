/*
 * tools/libtxt/edit.c - Editing and rendering
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

#include "libtxt.h"


static int do_edit(uint8_t *canvas, int width, int height,
    const struct edit *e, const char **error)
{
	struct image *img;
	struct font *font = NULL;
	int x = 0, y = 0, max = 0;
	int spc = 1;
	const char *s;
	int xo, yo;

	while (e) {
		switch (e->type) {
		case edit_string:
			for (s = e->u.s; *s; s++) {
				xo = draw_char(canvas, width, height,
				    font, *s, x, y);
				yo = char_height(font, *s);
				if (yo > max)
					max = yo;
				x += xo+spc;
			}
			break;
		case edit_font:
			free_font(font);
			img = load_image(e->u.s, error);
			if (!img)
				return 0;
			font = make_font(img, error);
			if (!font)
				goto fail;
			break;
		case edit_img:
			img = load_image(e->u.s, error);
			if (!img)
				return 0;
			xo = draw_image(canvas, width, height, img, x, y);
			free_image(img);
			x += xo;
			break;
		case edit_spc:
			spc = e->u.n;
			break;
		case edit_xoff:
			x += e->u.n;
			break;
		case edit_xpos:
			x = e->u.n;
			break;
		case edit_yoff:
			y += e->u.n;
			break;
		case edit_ypos:
			y = e->u.n;
			break;
		case edit_nl:
			x = 0;
			y += max+1;
			max = 0;
			break;
		default:
			abort();
		}
		e = e->next;
	}
	return 1;

fail:
	free_font(font);
	return 0;
}


void *apply_edits(int width, int height, const struct edit *e,
    const char **error)
{
	uint8_t *canvas;

	canvas = calloc(1, (width*height+7) >> 3);
	if (!canvas)
		abort();
	if (do_edit(canvas, width, height, e, error))
		return canvas;
	free(canvas);
	return NULL;
}
