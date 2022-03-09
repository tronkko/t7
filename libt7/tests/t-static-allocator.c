/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/static-allocator.h"
#include "t7/memory.h"

#undef NDEBUG
#include <assert.h>


/* Prototypes */
static struct allocator *allocate_my_allocator(void);
static void free_my_allocator(struct allocator *ap);
static int create_my_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable);
static void destroy_my_allocator(struct allocator *ap);

/* Custom allocator type */
static struct allocator_vtable def = {
	allocate_my_allocator,
	free_my_allocator,
	create_my_allocator,
	destroy_my_allocator,
	static_grab_memory,
	static_release_memory,
	static_resize_memory,
};
static const struct allocator_vtable *my_allocator = &def;

/* The single allocator instance */
static struct static_allocator singleton;

/* Static buffer */
static char buffer[1024];


int
main (void)
{
	/* Create allocator */
	struct allocator *ap = get_allocator(my_allocator);
	assert(ap != NULL);

	/* Allocate 1000 bytes */
	char *p1 = allocator_allocate_memory(ap, 1000);
	assert(p1 != NULL);

	/* Allocation of another 1000 bytes cannot succeed */
	char *p2 = allocator_allocate_memory(ap, 1000);
	assert(p2 == NULL);

	/* Reduce size to 100 bytes ... */
	p1 = allocator_resize_memory(ap, p1, 100);
	assert(p1 != NULL);

	/* ... after which there is room to allocate 800 bytes */
	p2 = allocator_allocate_memory(ap, 800);
	assert(p2 != NULL);

	/* Release both memory areas */
	allocator_free_memory(ap, p1);
	allocator_free_memory(ap, p2);

	/* Buffer can accommodiate 64 integers on a 64-bit system */
	char *ptrs[64];
	for (size_t i = 0; i < 64; i++) {
		ptrs[i] = allocator_allocate_memory(ap, sizeof (int));
		if (ptrs[i] == NULL) {
			assert(ptrs[i] != NULL);
		}
	}

	/* Release integers */
	for (size_t i = 0; i < 64; i++) {
		allocator_free_memory(ap, ptrs[i]);
	}

	return 0;
}


/* Allocate room for static allocator structure */
static struct allocator *allocate_my_allocator(void)
{
	/* Return pointer to sole allocator instance */
	return (struct allocator*) &singleton;
}


/* Release static allocator */
static void free_my_allocator(struct allocator *ap)
{
	(void) ap;
	/*NOP*/;
}


/* Initialize allocator */
static int create_my_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable)
{
	return create_static_allocator_with_buffer(
		ap, vtable, buffer, sizeof(buffer));
}


/* Uninitialize allocator */
static void destroy_my_allocator(struct allocator *ap)
{
	destroy_allocator(ap);
}


