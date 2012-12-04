/*
 * fw/led.h - LED control
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LED_H
#define LED_H

#include <stdint.h>

#include <avr/pgmspace.h>


#define	N_LEDS		64
#define	LED_BYTES	(N_LEDS/8)


void led_show(const uint8_t p[LED_BYTES]);
void led_show_pgm(const prog_uint8_t p[LED_BYTES]);
void led_off(void);
void led_init(void);

#endif /* !LED_H */
