/*
 * Licensed under a two-clause BSD-style license.
 * See LICENSE for details.
 */

#ifndef COMPAT_UTIL_H
#define COMPAT_UTIL_H

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#define error(...) (fprintf(stderr, "error: " __VA_ARGS__), -1)
#define die(...) \
	do { \
		fprintf(stderr, "fatal: " __VA_ARGS__); \
		exit(128); \
	} while (0)

#define maximum_signed_value_of_type(b) \
	(INTMAX_MAX >> (8 * (sizeof(intmax_t) - sizeof(b))))
#define signed_add_overflows(a, b) \
	((a) > maximum_signed_value_of_type(b) - (b))
#define unsigned_add_overflows(a, b) ((a) + (b) < (a))

/* rate of growth taken from git. */
static inline void alloc_grow(void **array, size_t nmemb,
				size_t *alloc, size_t elemsize)
{
	const size_t newalloc = (*alloc + 16) * 3 / 2;

	if (nmemb <= *alloc)
		return;
	*alloc = newalloc < nmemb ? nmemb : newalloc;
	*array = realloc(*array, *alloc * elemsize);
}

#define ALLOC_GROW(array, sz, alloc) \
	alloc_grow((void **) &array, sz, &alloc, sizeof(*array))

#endif
