/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_ALLOCATOR_H
#define T7_ALLOCATOR_H
#ifdef __cplusplus
extern "C" {
#endif

/* Forward-decl */
struct allocator;
struct allocator_vtable;

/* Get pointer to specific allocator */
struct allocator *get_allocator(const struct allocator_vtable *vtable);

/* Get pointer to default allocator */
struct allocator *get_default_allocator(void);

/* Create new allocator */
struct allocator *new_allocator(const struct allocator_vtable *vtable);

/* Delete allocator */
void delete_allocator(struct allocator *ap);

/* Allocate memory from allocator */
void *allocator_allocate_memory(struct allocator *ap, size_t n);

/* Resize allocated memory area */
void *allocator_resize_memory(struct allocator *ap, void *p, size_t n);

/* Release memory */
void allocator_free_memory(struct allocator *ap, void *p);

/* Allocate memory for default allocator_t structure */
struct allocator *allocate_allocator(void);

/* Release allocator_t structure */
void free_allocator(struct allocator *ap);

/* Initialize default allocator object */
int create_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable);

/* Un-initialize default allocator */
void destroy_allocator(struct allocator *ap);

/* Virtual functions in vtable */
typedef struct allocator *allocate_allocator_function(void);
typedef void free_allocator_function(struct allocator *ap);
typedef int create_allocator_function(
	struct allocator *ap, const struct allocator_vtable *vtable);
typedef void destroy_allocator_function(struct allocator *ap);
typedef void *grab_memory_function(struct allocator *ap, size_t n);
typedef void release_memory_function(struct allocator *ap, void *p);
typedef void *resize_memory_function(struct allocator *ap, void *p, size_t n);

/* Allocator type */
struct allocator_vtable {
	allocate_allocator_function *allocate;
	free_allocator_function *free;
	create_allocator_function *create;
	destroy_allocator_function *destroy;
	grab_memory_function *grab;
	release_memory_function *release;
	resize_memory_function *resize;
};

/* The allocator */
struct allocator {
	struct allocator *next;
	struct allocator *prev;
	const struct allocator_vtable *vtable;
};

/* Pointer to default allocator */
extern const struct allocator_vtable *default_allocator;

#ifdef __cplusplus
}
#endif
#endif /*T7_ALLOCATOR_H*/

