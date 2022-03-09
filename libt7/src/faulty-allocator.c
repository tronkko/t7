/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/faulty-allocator.h"
#include "t7/simulate-failure.h"
#include "t7/memory.h"
#include "t7/fixture.h"


/* Internal allocator functions */
static struct allocator *allocate_faulty_allocator(void);
static void free_faulty_allocator(struct allocator *ap);
static void *faulty_allocate_memory(struct allocator *ap, size_t n);
static void faulty_free_memory(struct allocator *ap, void *p);
static void *faulty_resize_memory(struct allocator *ap, void *p, size_t n);

/* Allocator type */
static struct allocator_vtable def1 = {
	allocate_faulty_allocator,
	free_faulty_allocator,
	create_allocator,
	destroy_allocator,
	faulty_allocate_memory,
	faulty_free_memory,
	faulty_resize_memory,
};
const struct allocator_vtable *faulty_allocator = &def1;

/* Static allocator object */
static struct allocator singleton;

/* Default fixture for testing */
static fixture_t def2 = {
	get_faulty_allocator
};
fixture_t *test_fixture = &def2;


/* Get allocator for test_fixture */
static struct allocator *get_faulty_allocator(fixture_t *fp)
{
	(void) fp;
	return get_allocator(faulty_allocator);
}


/* Allocate allocator structure */
static struct allocator *allocate_faulty_allocator(void)
{
	/* Return reference to singleton instance */
	return &singleton;
}


/* Release allocator */
static void free_faulty_allocator(struct allocator *ap)
{
	(void) ap;
	/*NOP*/;
}


/* Allocate memory from faulty allocator */
static void *faulty_allocate_memory(struct allocator *ap, size_t n)
{
	(void) ap;

	void *p;
	if (!simulate_failure()) {
		/* No simulation, allocate memory from system */
		p = system_allocate_memory(n);
	} else {
		/* Simulated failure */
		p = NULL;
	}
	return p;
}


/* Release memory back to faulty allocator */
static void faulty_free_memory(struct allocator *ap, void *p)
{
	(void) ap;

	/* Return memory directly to system */
	system_free_memory(p);
}


/* Resize memory region */
static void *faulty_resize_memory(struct allocator *ap, void *p, size_t n)
{
	(void)ap;

	void *q;
	if (!simulate_failure()) {
		/* No simulation, allocate memory from system */
		q = system_resize_memory(p, n);
	} else {
		/* Simulated failure */
		q = NULL;
	}
	return q;
}


