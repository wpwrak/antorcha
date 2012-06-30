/*
 * tools/ant-cl/plot.c - Sample plot
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "plot.h"


#define XRES    1024
#define YRES    1024

#define	RADIUS	3

#define	OLD_RGBA	0xffffff20
#define	NEW_RGBA	0xffffffff


static SDL_Surface *surf;
static int first = 1;
static int last_x, last_y;

static SDL_Surface *back;
static SDL_Rect back_rect = {
	.x = 0,
	.y = 0,
	.w = 2*RADIUS+1,
	.h = 2*RADIUS+1,
};

static SDL_Rect front_rect = {
	.w = 2*RADIUS+1,
	.h = 2*RADIUS+1,
};


int plot(int x, int y)
{
	SDL_Event event;

	if (!first) {
		SDL_BlitSurface(back, &back_rect, surf, &front_rect);
		SDL_LockSurface(surf);
		filledCircleColor(surf, last_x, last_y, RADIUS, OLD_RGBA);
		SDL_UnlockSurface(surf);
	}

	front_rect.x = x;
	front_rect.y = y;
	x += RADIUS;
	y += RADIUS;

	SDL_BlitSurface(surf, &front_rect, back, &back_rect);
	SDL_LockSurface(surf);
	filledCircleColor(surf, x, y, RADIUS, NEW_RGBA);
	SDL_UnlockSurface(surf);

	if (!first)
		SDL_UpdateRect(surf,
		    last_x-RADIUS, last_y-RADIUS, 2*RADIUS+1, 2*RADIUS+1);
	SDL_UpdateRect(surf, x-RADIUS, y-RADIUS, 2*RADIUS+1, 2*RADIUS+1);

	first = 0;
	last_x = x;
	last_y = y;

	while (SDL_PollEvent(&event))
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_c:
				SDL_LockSurface(surf);
				SDL_FillRect(surf, NULL,
				    SDL_MapRGB(surf->format, 0, 0, 0));
				SDL_UpdateRect(surf, 0, 0, 0, 0);
				SDL_UnlockSurface(surf);
				break;
			case SDLK_q:
				return 0;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			return 0;
		default:
			break;
		}

	return 1;
}


void plot_init(void)
{
	const SDL_PixelFormat *f;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_init: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	surf = SDL_SetVideoMode(XRES+2*RADIUS+1, YRES+2*RADIUS+1, 0,
	    SDL_SWSURFACE);
	if (!surf) {
		fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
		exit(1);
	}

	f = surf->format;
	back = SDL_CreateRGBSurface(SDL_SWSURFACE, 2*RADIUS+1, 2*RADIUS+1,
	    f->BitsPerPixel,  f->Rmask, f->Gmask, f->Bmask, f->Amask);
	if (!back) {
		fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
		exit(1);
	}
}
