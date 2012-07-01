/*
 * tools/libtxt/util.h - Utility functions
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static inline void *alloc_size(size_t size)
{
	void *tmp = malloc(size);

	if (!tmp)
		abort();
	return tmp;
}


#define alloc_type(t) ((t *) alloc_size(sizeof(t)))


/*
 * @@@ __attribute__((used)) is an ugly wait to get rid of the "unused
 * function" warning. (The "unused" attribute doesn't do the trick.)
 */
 
static const char * __attribute__((used)) alloc_sprintf(const char *fmt, ...)
{
	va_list ap;
	char *tmp, *res;
	int n;

	va_start(ap, fmt);
	n = vasprintf(&tmp, fmt, ap);
	va_end(ap);
	if (n < 0)
		abort();
	res = malloc(n+1);
	if (!res)
		abort();
	memcpy(res, tmp, n+1);
	return res;
}

#endif /* !UTIL_H */
