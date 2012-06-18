/*
 * fw/hash.h - Secure hash
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef HASH_H
#define	HASH_H

#include <stdbool.h>
#include <stdint.h>


void hash_init(void);
void hash_merge(const uint8_t *buf, uint8_t len);
void hash_end(void);
bool hash_eq(const uint8_t *buf, uint8_t len, uint8_t off);
void hash_cp(uint8_t *buf, uint8_t len, uint8_t off);

#endif /* !HASH_H */
