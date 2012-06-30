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

struct image;

struct font;

struct edit {
	enum edit_type {
		edit_string,
		edit_font,
		edit_spc,
		edit_xoff,
		edit_xpos,
		edit_yoff,
		edit_ypos,
		edit_nl,
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
 * <SPC offset>
 * <X+offset> <X-offset> <X=pos>
 * <Y+offset> ...
 *
 * Empty lines are ignored. The newline of a line containing only markup is
 * also ignored.
 *
 * Newline leaves one blank row between text lines.
 */

struct image *load_image(const char *name, const char **error);
void free_image(struct image *img);

struct font *make_font(struct image *img, const char **error);
void free_font(struct font *font);

int draw_char(void *canvas, int width, int height,
    const struct font *font, char c, int x, int y);
int char_height(const struct font *font, char c);

struct edit *text2edit(const char *s);
char *edit2text(const struct edit *e);

void *apply_edits(int width, int height, const struct edit *e,
    const char **error);

#endif /* !LIBTXT_H */
