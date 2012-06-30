/*
 * tools/libtxt/libtxt.h - Text processing functions
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LIBTXT_H
#define LIBTXT_H

struct edit {
	enum edit_type {
		edit_string,
		edit_font,
		edit_xoff,
		edit_xpos,
		edit_yoff,
		edit_ypos,
	} type;
	union {
		const char *s;
		int n;
	} u;
	struct edit *next;
};

/*
 * Markup:
 *
 * <FONT fontname>
 * <IMG imagefile>
 * <X+offset> <X-offset> <X=pos>
 * <Y+offset> ...
 *
 * Empty lines are ignored. The newline of a line containing only markup is
 * also ignored.
 *
 * Newline leaves one 
 */

const char *load_font(const char *name);
int draw_char(void *canvas, int width, int height, char c, int x, int y);

struct edit *text2edit(const char *s);
char *edit2text(const struct edit *e);

void *apply_edits(int width, int height, const struct edit *e);

#endif /* !LIBTXT_H */
