/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_STATIC_ALLOCATOR_H
#define T7_STATIC_ALLOCATOR_H
#include "t7/allocator.h"
#ifdef __cplusplus
extern "C" {
#endif


/* Forward-decl */
struct static_node;
struct static_allocator;


/* Initialize static allocator with buffer */
int create_static_allocator_with_buffer(
	struct allocator *ap, const struct allocator_vtable *vtable,
	char *buffer, size_t size);

/* Static allocator type */
extern const struct allocator_vtable *static_allocator;


/* Structure of static allocator */
struct static_allocator {
	/* Base allocator, must be first member of the structure */
	struct allocator base;

	/* Pointer to next memory node to check */
	struct static_node *start;

	/* Pointer to memory buffer */
	char *buffer;

	/* Total size of memory buffer in bytes */
	size_t size;
};

/* Structure of internal memory node */
struct static_node {
	size_t size;
};


/* Virtual functions */
struct allocator *allocate_static_allocator(void);
void free_static_allocator(struct allocator *ap);
int create_static_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable);
void destroy_static_allocator(struct allocator *ap);
void *static_grab_memory(struct allocator *ap, size_t n);
void static_release_memory(struct allocator *ap, void *p);
void *static_resize_memory(struct allocator *ap, void *p, size_t n);


#ifdef __cplusplus
}
#endif
#endif /*T7_STATIC_ALLOCATOR_H*/

