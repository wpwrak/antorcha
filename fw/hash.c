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


#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "hash.h"


#define	HASH_SIZE	128


static uint8_t hash[HASH_SIZE];


void hash_init(void)
{
	memset(hash, 0, HASH_SIZE);
}


void hash_merge(const uint8_t *buf, uint8_t len)
{
	uint8_t i;

	for (i = 0; i != len; i++)
		hash[i & (HASH_SIZE-1)] ^= buf[i];
}


void hash_end(void)
{
}


bool hash_eq(const uint8_t *buf, uint8_t len, uint8_t off)
{
	uint8_t i;

	for (i = 0; i != len; i++)
		if (hash[(off+i) & (HASH_SIZE-1)] != buf[i])
			return 0;
	return 1;
}


void hash_cp(uint8_t *buf, uint8_t len, uint8_t off)
{
	memcpy(buf, hash+off, len);
}
