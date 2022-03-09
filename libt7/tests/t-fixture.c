/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/memory.h"
#include "t7/fixture.h"
#include "t7/terminate.h"
#include "t7/static-allocator.h"
#include "t7/thread.h"

#undef NDEBUG
#include <assert.h>


/* Custom fixture & thread functions */
static struct allocator *get_my_allocator(fixture_t *fp);
static int my_run_thread (thread_t *tp);

/* Define custom fixture type */
static fixture_t static_fixture = {
	get_my_allocator
};
static fixture_t *my_fixture = &static_fixture;

/* Define custom thread type */
static thread_type_t def1 = {
	allocate_thread,
	free_thread,
	create_thread,
	destroy_thread,
	my_run_thread
};
static thread_type_t *my_thread = &def1;


/* Test functions */
void test_static_allocator(void);
void test_allocator(void);
void test_thread(void);


int main(void)
{
	test_static_allocator();
	test_thread();
	return 0;
}


/* Test change of fixture */
void
test_static_allocator(void)
{
	fixture_t *fp;
	fixture_t *orig;
	struct allocator *ap;

	/* Get pointer to original fixture */
	orig = get_fixture();

	/* Set up custom fixture for the main thread */
	set_fixture(my_fixture);

	/* Retrieve the current fixture and make sure allocator was changed */
	fp = get_fixture();
	assert(fp != NULL);
	ap = get_default_allocator();
	assert(ap == get_allocator(static_allocator));

	/* Play with some memory */
	test_allocator();

	/* Restore original fixture */
	set_fixture(orig);

	/* Make sure that allocator was restored */
	assert(orig == get_fixture());

	/* Repeat memory test with standard allocator */
	test_allocator();
}


/* Test thread-specific fixture */
void
test_thread (void)
{
	thread_t *tp;
	int result;
	fixture_t *orig;

	/* Get original fixture */
	orig = get_fixture();

	/* Create thread */
	tp = new_thread(my_thread);
	assert(tp != NULL);
	result = start_thread(tp);
	assert(result != 0);

	/* Wait for thread to finish */
	result = join_thread(tp);
	assert(result == 1);
	delete_thread(tp);

	/* Make sure that fixture of the main thread is intact */
	assert(get_fixture() == orig);
}


/* Allocate memory using default allocator */
void
test_allocator (void)
{
	/* Allocate 10 bytes of memory */
	char *p = allocate_memory(10);
	assert(p != NULL);

	/* Initialize memory area */
	zero_memory(p, 10);

	/* Allocate another 10 bytes of memory */
	char *p2 = allocate_memory(20);
	assert(p2 != NULL);

	/* Initialize second memory area */
	fill_memory(p2, 'a', 20);

	/* Make sure that first memory area is intact */
	for (size_t i = 0; i < 10; i++) {
		assert(p[i] == '\0');
	}

	/* Release second memory with no ill effects */
	free_memory(p2);

	/* Release first memory with no ill effects */
	free_memory(p);
}


/* Get allocator for my_fixture */
static struct allocator *get_my_allocator(fixture_t *fp)
{
	(void) fp;
	return get_allocator(static_allocator);
}


/* Thread main function */
static int my_run_thread(thread_t *tp)
{
	fixture_t *fp;
	struct allocator *ap;

	/* Ignore parameter */
	(void) tp;

	/* Set up custom fixture for the sub-thread */
	set_fixture(my_fixture);

	/* Retrieve the current fixture and make sure allocator was changed */
	fp = get_fixture();
	assert(fp != NULL);
	ap = get_default_allocator();
	assert(ap == get_allocator(static_allocator));

	/* Play with some memory */
	test_allocator();

	/* Return without restoring the original fixture */
	return 1;
}

