/*
 * tools/ant-gui/ant-gui.c - Antorcha GUI
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

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include <libant/libant.h>


#define	XRES	320	/* canvas width */
#define	YRES	240	/* canvas height */

#define	W	80	/* image width */
#define	H	16	/* image height */

#define	NX	(XRES/W)
#define	NY	(YRES/H)


#define	FG_SEL_RGBA	0x000000ff
#define	BG_SEL_RGBA	0xffffffff
#define	FG_NORM_RGBA	0xffffffff
#define	BG_NORM_RGBA	0x000000ff
#define	FG_ACT_RGBA	0xff2020ff


struct img {
	uint8_t *canvas;
	const char *path;
};


static void render(SDL_Surface *s, struct img *img, int x, int y,
    int sel, int act)
{
	int ix, iy;

	uint32_t fg = act ? FG_ACT_RGBA : sel ? FG_SEL_RGBA : FG_NORM_RGBA;
	uint32_t bg = sel ? BG_SEL_RGBA : BG_NORM_RGBA;

	for (iy = 0; iy != H; iy++)
		for (ix = 0; ix != W; ix++)
			if (img->canvas[(iy*W+ix) >> 3] & (1 << (ix & 7)))
				pixelColor(s, x+ix, y+iy, fg);
			else
				pixelColor(s, x+ix, y+iy, bg);
}


static void gui(struct img *img, int n)
{
	SDL_Surface *surf;
	SDL_Event event;
	int x = 0, y = 0, y_top = 0;
	int dx, dy, upload;
	int ix, iy, i;
	int active = -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_init: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	surf = SDL_SetVideoMode(XRES, YRES, 0, SDL_SWSURFACE);
	if (!surf) {
		fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
		exit(1);
	}

	while (1) {
		dx = dy = upload = 0;
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
				if (y > 0)
					dy = -1;
				break;
			case SDLK_DOWN:
				if ((y+1)*NX+x < n)
					dy = 1;
				break;
			case SDLK_RIGHT:
				dx = 1;
				break;
			case SDLK_LEFT:
				dx = -1;
				break;
			case SDLK_RETURN:
				upload = 1;
				break;
			case SDLK_q:
				return;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			return;
		default:
			break;
		}

		x = (x+NX+dx) % NX;
		y += dy;
		if (y*NX+x >= n)
			x = n-y*NX-1;
		if (y < y_top)
			y_top = y;
		if (y >= y_top+NY)
			y_top = y-NY+1;

		if (upload) {
			i = y*NX+x;
			active = i;
		}

		SDL_LockSurface(surf);
	        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 0, 0));
		for (iy = 0; iy != NY; iy++)
			for (ix = 0; ix != NX; ix++) {
				i = iy*NX+ix;
				if (i >= n)
					break;
				render(surf, img+i, ix*W, (iy-y_top)*H,
				    ix == x && iy == y, i == active);
			}
		SDL_UnlockSurface(surf);
		SDL_UpdateRect(surf, 0, 0, 0, 0);
	}
}


static void *generate(const char *path)
{
	FILE *file;
	char buf[100];
	struct edit *edits = NULL, **last = &edits;
	const char *err;
	char *nl;
	void *res;

	file = fopen(path, "r");
	if (!file) {
		perror(path);
		exit(1);
	}
	while (fgets(buf, sizeof(buf), file)) {
		nl = strchr(buf, '\n');
		if (nl)
			*nl = 0;
		if (edits) {
			*last = malloc(sizeof(struct edit));
			if (!*last)
				abort();
			(*last)->type = edit_nl;
			last = &(*last)->next;
		}
		*last = text2edit(buf, &err);
		if (!*last) {
			fprintf(stderr, "\"%s\": %s\n", buf, err);
			exit(1);
		}
		while (*last)
			last = &(*last)->next;
	}
	fclose(file);
	res = apply_edits(W, H, edits, &err);
	if (!res) {
		fprintf(stderr, "%s\n", err);
		exit(1);
	}
	free_edits(edits);
	return res;
}


int main(int argc, char **argv)
{
	int n = argc-1;
	struct img img[n];
	int i;

	for (i = 0; i != n; i++) {
		img[i].canvas = generate(argv[i+1]);
		img[i].path = argv[i+1];
	}
	gui(img, n);
	exit(1);
}
