/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/allocator.h"
#include "t7/static-allocator.h"
#include "t7/memory.h"

#undef NDEBUG
#include <assert.h>


/* Test functions */
static void test_allocator(struct allocator *ap);


int
main (void)
{
	struct allocator *ap;

	/* Execute test using default allocator */
	ap = get_allocator(default_allocator);
	assert(ap != NULL);
	test_allocator(ap);

	/* Execute test using static allocator */
	ap = get_allocator(static_allocator);
	assert(ap != NULL);
	test_allocator(ap);

	/* NULL allocator may be destroyed without ill effects */
	delete_allocator(NULL);
	return 0;
}


/* Test allocator */
static void test_allocator(struct allocator *ap)
{
	assert(ap != NULL);

	/* Allocate and release memory areas repeatedly */
	for (size_t count = 0; count < 10000; count++) {
		/* Trying to allocate zero bytes may result to NULL pointer */
		char *p1 = ap->vtable->grab(ap, 0);

		/* Trying to release a NULL pointer does not lead to error */
		ap->vtable->release(ap, p1);

		/* Allocate 15 bytes using allocator */
		p1 = ap->vtable->grab(ap, 15);
		assert (p1 != NULL);

		/* Initialize memory */
		fill_memory(p1, '!', 15);
		for (size_t i = 0; i < 15; i++) {
			assert(p1[i] == '!');
		}

		/* Allocate 12 bytes */
		char *p2 = ap->vtable->grab(ap, 12);
		assert(p2 != NULL);

		/* Initialize second memory area */
		fill_memory(p2, 'a', 12);
		for (size_t i = 0; i < 12; i++) {
			assert(p2[i] == 'a');
		}

		/* Enlarge first memory area to 55 bytes */
		char *tmp = ap->vtable->resize(ap, p1, 55);
		assert(tmp != NULL);
		p1 = tmp;

		/* Make sure that first memory area is intact */
		for (size_t i = 0; i < 15; i++) {
			assert(p1[i] == '!');
		}

		/* Fill first memory area */
		fill_memory(p1, 'x', 55);
		for (size_t i = 0; i < 55; i++) {
			assert(p1[i] == 'x');
		}

		/* Make sure that second memory area is intact */
		for (size_t i = 0; i < 12; i++) {
			assert(p2[i] == 'a');
		}

		/* Shrink first memory area to 10 bytes */
		tmp = ap->vtable->resize(ap, p1, 10);
		assert(tmp != NULL);
		p1 = tmp;

		/* Make sure that first memory area is intact */
		for (size_t i = 0; i < 10; i++) {
			assert(p1[i] == 'x');
		}

		/* Make sure that second memory area is intact */
		for (size_t i = 0; i < 12; i++) {
			assert(p2[i] == 'a');
		}

		/* Release memory areas */
		ap->vtable->release(ap, p1);
		ap->vtable->release(ap, p2);
	}
}


