/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/fixture.h"
#include "t7/allocator.h"
#include "t7/memory.h"
#include "t7/exit-handler.h"
#include "t7/critical-section.h"


/* Get pointer to allocator */
static struct allocator *find_allocator(
	const struct allocator_vtable *vtable);

/* Default heap allocator */
static void *default_allocate_memory(struct allocator *ap, size_t n);
static void default_free_memory(struct allocator *ap, void *p);
static void *default_resize_memory(struct allocator *ap, void *p, size_t n);

/* Cleanup function */
static void cleanup (void);


/* Default allocator */
static struct allocator_vtable def = {
	allocate_allocator,
	free_allocator,
	create_allocator,
	destroy_allocator,
	default_allocate_memory,
	default_free_memory,
	default_resize_memory,
};
const struct allocator_vtable *default_allocator = &def;

/* List of allocators */
static struct allocator head;
static struct allocator tail;

/* True if allocator list initialized */
static int initialized = 0;


/* Get reference to default allocator */
struct allocator *get_default_allocator(void)
{
	/* Get current fixture (will terminate app on failure) */
	fixture_t *fp = get_fixture();
	assert(fp != NULL);

	/* Retrieve allocator from fixture */
	assert(fp->get_fixture_allocator != NULL);
	return fp->get_fixture_allocator(fp);
}


/* Get reference to given allocator */
struct allocator *get_allocator(const struct allocator_vtable *vtable)
{
	struct allocator *ap;
	enter_critical();

	/* Is the module initialized? */
	if (initialized) {
		/* Yes, find/create allocator */
		ap = find_allocator(vtable);
	} else {
		/* No, register exit function to clean up variables */
		if (exit_handler(cleanup, 20)) {
			/* Initialize head node */
			head.next = &tail;
			head.prev = NULL;
			head.vtable = NULL;

			/* Initialize tail node */
			tail.next = NULL;
			tail.prev = &head;
			tail.vtable = NULL;

			/* Mark initialization as done */
			initialized = 1;

			/* Create first allocator */
			ap = find_allocator(vtable);
		} else {
			/* Cannot install exit function */
			ap = NULL;
		}
	}

	leave_critical();
	return ap;
}


/* Find allocator by type */
static struct allocator *find_allocator(const struct allocator_vtable *vtable)
{
	/* Loop through allocators in list */
	struct allocator *ap = head.next;
	while (ap->next != NULL) {
		/* Exit now if this is the allocator we are looking for */
		if (ap->vtable == vtable)
			return ap;

		/* Check next allocator in list */
		ap = ap->next;
	}

	/* Create new allocator */
	ap = new_allocator(vtable);
	if (!ap)
		return NULL;

	/* Add allocator to end of list */
	ap->next = &tail;
	ap->prev = tail.prev;
	tail.prev->next = ap;
	tail.prev = ap;
	return ap;
}


/* Create new allocator */
struct allocator *new_allocator(const struct allocator_vtable *vtable)
{
	assert(vtable != NULL);
	assert(vtable->allocate != NULL);
	assert(vtable->free != NULL);
	assert(vtable->create != NULL);
	assert(vtable->destroy != NULL);
	assert(vtable->grab != NULL);
	assert(vtable->release != NULL);
	assert(vtable->resize != NULL);

	/* Allocate memory for allocator */
	struct allocator *ap = vtable->allocate();
	if (!ap)
		goto exit_null;

	/* Initialize custom attributes */
	if (!vtable->create(ap, vtable))
		goto exit_clean;

	/* Allocator must be initialized with virtual table vtable */
	assert(ap->vtable == vtable);

	/* Success */
	return ap;

exit_clean:
	/* Initialization failed */
	vtable->free(ap);

exit_null:
	/* Error */
	return NULL;
}


/* Delete allocator with resources */
void delete_allocator(struct allocator *ap)
{
	if (!ap)
		return;

	/* Get pointer to virtual table */
	const struct allocator_vtable *vtable = ap->vtable;
	assert(vtable != NULL);

	/* Detach allocator from list */
	if (ap->next) {
		assert(ap->next->prev == ap);
		ap->next->prev = ap->prev;
	}
	if (ap->prev) {
		assert(ap->prev->next == ap);
		ap->prev->next = ap->next;
	}

	/* Destroy custom allocator */
	assert(vtable->destroy != NULL);
	vtable->destroy(ap);

	/* Release allocator object */
	assert(vtable->free != NULL);
	vtable->free(ap);
}


/* Allocate memory from allocator */
void *allocator_allocate_memory(struct allocator *ap, size_t n)
{
	assert(ap != NULL);

	/* Return null if asking for zero bytes */
	if (!n)
		return NULL;

	/* Allocate memory through allocator */
	assert(ap->vtable->grab != NULL);
	return ap->vtable->grab(ap, n);
}


/* Resize memory using allocator */
void *allocator_resize_memory(struct allocator *ap, void *p, size_t n)
{
	assert (ap != NULL);

	/* Do we have a memory area to resize? */
	void *q;
	if (p) {
		/* Yes, is the new size greater than zero? */
		if (n) {
			/* Yes, resize memory through allocator */
			assert(ap->vtable->resize != NULL);
			q = ap->vtable->resize(ap, p, n);
		} else {
			/* Resizing to zero bytes => release memory area */
			assert(ap->vtable->release != NULL);
			ap->vtable->release(ap, p);
			q = NULL;
		}
	} else {
		if (n) {
			/* Resizing null pointer => allocate memory area */
			assert(ap->vtable->grab != NULL);
			q = ap->vtable->grab(ap, n);
		} else {
			/* Resizing NULL pointer to zero bytes?!? */
			q = NULL;
		}
	}
	return q;
}


/* Release memory using allocator */
void allocator_free_memory(struct allocator *ap, void *p)
{
	assert (ap != NULL);

	/* Exit now if no memory allocated */
	if (!p)
		return;

	/* Release memory through allocator */
	assert(ap->vtable->release != NULL);
	ap->vtable->release(ap, p);
}


/* Allocate memory for default allocator */
struct allocator *allocate_allocator (void)
{
	return (struct allocator*)
		system_allocate_memory(sizeof(struct allocator));
}


/* Release default allocator */
void free_allocator(struct allocator *ap)
{
	system_free_memory(ap);
}


/* Initialize default allocator */
int create_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable)
{
	assert(ap != NULL);
	assert(vtable != NULL);

	ap->next = NULL;
	ap->prev = NULL;
	ap->vtable = vtable;
	return 1;
}


/* Un-initialize default allocator */
void destroy_allocator(struct allocator *ap)
{
#ifndef NDEBUG
	/* Reset fields */
	ap->next = (struct allocator*) -1;
	ap->prev = (struct allocator*) -1;
	ap->vtable = (struct allocator_vtable*) -1;
#else
	/* Silence warnings about unused argument */
	(void) ap;
#endif
}


/* Allocate memory from default allocator */
static void *default_allocate_memory(struct allocator *ap, size_t n)
{
	(void) ap;
	return system_allocate_memory(n);
}


/* Release memory from default allocator */
static void default_free_memory (struct allocator *ap, void *p)
{
	(void) ap;
	system_free_memory(p);
}


/* Resize memory region */
static void *default_resize_memory(struct allocator *ap, void *p, size_t n)
{
	(void) ap;
	return system_resize_memory(p, n);
}


/* Release allocators at program exit */
static void cleanup (void)
{
	assert(initialized);

	/*
	 * Loop through allocators and destroy them.
	 *
	 * Be ware that the code below assumes that no other threads are
	 * running.  If some threads are still running, then they might be
	 * using the allocator being destroyed which causes the program to
	 * crash!
	 *
	 * Also note that the code below assumes that allocators are mostly
	 * independent of each other.  If an allocator calls another allocator
	 * to do its bidding, then the code below will crash due if the
	 * underlying allocator has already been destroyed!  In order to
	 * alleviate this problem, we destroy allocators in the reverse order
	 * of construction.
	 */
	struct allocator *ap = tail.prev;
	while (ap->prev != NULL) {
		/* Grab pointer to previous allocator */
		struct allocator *prev = ap->prev;

		/* Destroy allocator and detach it from list */
		delete_allocator(ap);

		/* Continue with previous allocator */
		ap = prev;
	}

	/* Ensure that the list is empty */
	assert(head.next == &tail);
	assert(tail.prev == &head);
}

