/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/allocator.h"
#include "t7/memory.h"
#include "t7/static-allocator.h"
#include "t7/critical-section.h"


/* Internal functions */
static size_t roundup(size_t n);

static int is_free(struct static_node *node);

static struct static_node *get_successor(
	struct static_allocator *map, struct static_node *node);

static size_t get_size(
	struct static_allocator *map, struct static_node *node);

static void *allocate_node(
	struct static_allocator *map, struct static_node *node,
	size_t new_size);

static void *relocate_node(
	struct allocator *ap, struct static_node *node, size_t n);


/* Virtual table for allocator having dynamically allocated buffer */
static struct allocator_vtable def1 = {
	allocate_static_allocator,
	free_static_allocator,
	create_static_allocator,
	destroy_static_allocator,
	static_grab_memory,
	static_release_memory,
	static_resize_memory
};
const struct allocator_vtable *static_allocator = &def1;


/* Allocate room for static allocator object */
struct allocator *allocate_static_allocator(void)
{
	return system_allocate_memory(sizeof(struct static_allocator));
}


/* Release static allocator object */
void free_static_allocator(struct allocator *ap)
{
	system_free_memory(ap);
}


/* Initialize static allocator with statically allocated buffer */
int create_static_allocator_with_buffer(
	struct allocator *ap, const struct allocator_vtable *vtable,
	char *buffer, size_t size)
{
	/* Buffer must be provided */
	assert(buffer != NULL);

	/* Buffer must be aligned to 4 or 8-byte boundary */
	assert((((size_t) buffer) & (sizeof(struct static_node) - 1)) == 0);

	/* Size must be multiple of 16 to ensure that all nodes are valid */
	assert((size & 0xf) == 0);

	/* Initialize standard fields */
	if (!create_allocator(ap, vtable))
		return /*error*/0;

	/* Convert pointer to static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Save buffer data */
	map->buffer = buffer;
	map->size = size;

	/* Reset memory buffer (for debugging) */
#ifndef NDEBUG
	fill_memory(buffer, 0xCC, size);
#endif

	/* Create a free memory node at the beginning of the buffer */
	struct static_node *node = (struct static_node*) buffer;
	node->size = size;

	/* Start with the initialized node */
	map->start = node;
	return /*success*/ 1;
}


/* Initialize static allocator with dynamically allocated buffer */
int create_static_allocator(
	struct allocator *ap, const struct allocator_vtable *vtable)
{
	/* Pick a suitable size for buffer */
	size_t size = 1024 * 1024;

	/* Allocate room for buffer */
	char *buffer = system_allocate_memory(size);
	if (!buffer)
		return /*error*/ 0;

	/* Initialize standard fields */
	if (!create_allocator(ap, vtable))
		goto exit_clean;

	/* Initialize static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Save buffer */
	map->buffer = buffer;
	map->size = size;

	/* Reset memory buffer (for debugging) */
#ifndef NDEBUG
	fill_memory(buffer, 0xCC, size);
#endif

	/* Create a free memory node at the beginning of the buffer */
	struct static_node *node = (struct static_node*) buffer;
	node->size = size;

	/* Start with the initialized node */
	map->start = node;
	return /*success*/ 1;

exit_clean:
	/* Release buffer */
	system_free_memory(buffer);
	return /*error*/ 0;
}


/* Un-initialize static allocator having dynamically allocated buffer */
void destroy_static_allocator(struct allocator *ap)
{
	/* Convert allocator to static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Release buffer */
	if (map->buffer) {
		/* Reset memory buffer (for debugging) */
#ifndef NDEBUG
		fill_memory(map->buffer, 0xFF, map->size);
#endif

		/* Release buffer */
		system_free_memory(map->buffer);
	}

	/* Reset fields */
#ifndef NDEBUG
	map->start = (struct static_node*) -1;
	map->buffer = (char*) -1;
	map->size = (size_t) -1;
#endif
}


/* Allocate memory from static allocator */
void *static_grab_memory(struct allocator *ap, size_t n)
{
	/* Round the size up to ensure proper alignment of data types */
	size_t new_size = roundup(n);

	/* Lock out other threads */
	enter_critical();

	/* Convert allocator to static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Get pointer to a memory node */
	struct static_node *node = map->start;

	/*
	 * Loop through memory nodes and pick the first free memory node that
	 * is large enough to satisfy the request.
	 */
	void *result = NULL;
	do {
		/* Does the node satisfy the request? */
		if (is_free(node) && new_size <= get_size(map, node)) {
			/* Yes, allocate memory from node */
			result = allocate_node(map, node, new_size);
			break;
		}

		/* Skip to next node */
		node = get_successor(map, node);
	} while (node != map->start);

	leave_critical();
	return result;
}


/* Release memory back to static allocator */
void static_release_memory(struct allocator *ap, void *p)
{
	assert(p != NULL);

	/* Lock out other threads */
	enter_critical();

	/* Convert allocator to static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Construct pointer to memory node */
	struct static_node *node = &((struct static_node*) p)[-1];

	/* Ensure that memory was allocated using this allocator */
	assert((struct static_node*) map->buffer <= node);
	assert(node + 1 <= (struct static_node*) (map->buffer + map->size));

	/* Make sure that memory is allocated (avoid double free) */
	assert((node->size & 1u) != 0);

	/* Reset first bit to release the memory */
	node->size = (node->size & ~1u);

	/* Reset memory area (for debugging) */
#ifndef NDEBUG
	size_t n = node->size - sizeof(*node);
	assert(((char*) p) + n <= map->buffer + map->size);
	fill_memory(p, 0xFF, n);
#endif

	/* Start the search from this node when we need more memory */
	if (node < map->start) {
		map->start = node;
	}

	leave_critical();
}


/* Resize memory region */
void *static_resize_memory(struct allocator *ap, void *p, size_t n)
{
	assert(p != NULL);

	/* Round the size up to ensure proper alignment of data types */
	size_t new_size = roundup(n);

	/* Lock out other threads */
	enter_critical();

	/* Convert pointer to static allocator */
	struct static_allocator *map = (struct static_allocator*) ap;

	/* Construct pointer to memory node in question */
	struct static_node *node = &((struct static_node*) p)[-1];

	/* Ensure that memory was allocated using this allocator */
	assert((struct static_node*) map->buffer <= node);
	assert(node + 1 <= (struct static_node*) (map->buffer + map->size));

	/* Avoid handling already freed memory */
	assert((node->size & 1) != 0);

	/* Compute pointer past the last valid memory node */
	struct static_node *end =
		(struct static_node*) (map->buffer + map->size);

	/* Get size of this node (including header) */
	size_t available = (node->size & ~1u);

	/* Construct pointer to next node */
	struct static_node *next =
		(struct static_node*) (((char*) node) + available);

	/* Compute the size of this node plus successive free nodes */
	while (next != end && (next->size & 1u) == 0) {
		/* Include successive node in size */
		available += next->size;

		/* Construct pointer to next node */
		next = (struct static_node*) (((char*) node) + available);
	}

	/* Is the combined memory area large enough? */
	void *result;
	if (new_size <= available) {
		/* Yes, combine nodes and allocate room from the start */
		node->size = available;
		result = allocate_node(map, node, new_size);
	} else {
		/*
		 * The combined memory node cannot satisfy the request and we
		 * must relocate the memory area to another address.
		 */
		result = relocate_node(ap, node, n);
	}

	leave_critical();
	return result;
}


/*
 * Round the size up to ensure proper alignment of data types and to preserve
 * space for static node.
 *
 * For example, if the caller is requesting one byte of memory, we need to
 * allocate at least 8 bytes in order to align structures on a 32-bit
 * architecture.
 */
static size_t roundup(size_t n)
{
	size_t new_size;
	if (n < sizeof(struct static_node)) {
		/* Align to 4-byte (32-bit) or 8-byte boundary (64-bit) */
		new_size = sizeof(struct static_node) * 2;
	} else {
		/* Align to 8-byte boundary */
		new_size = sizeof(struct static_node) + ((n + 7u) & ~7u);
	}
	assert(new_size >= n);
	assert(new_size > sizeof(struct static_node));
	assert((new_size & 1) == 0);
	return new_size;
}


/* Get size of node */
static size_t get_size(
	struct static_allocator *map, struct static_node *node)
{
	/* Node must reside in buffer */
	assert((struct static_node*) map->buffer <= node);
	assert(node + 1 <= (struct static_node*) (map->buffer + map->size));

	/* Size must be valid */
	assert(node->size >= sizeof(struct static_node));
	assert(((char*)node) + (node->size & ~1u) <= map->buffer + map->size);

	/*
	 * Is the node allocated?
	 *
	 * Note that size and allocation status are packed into single size_t
	 * variable.  The least significant bit is set if node is allocated
	 * and zero otherwise.  For example, number 33 refers to an allocated
	 * 32-byte memory region whereas 32 refers to a free 32-byte memory
	 * region.
	 */
	if ((node->size & 1u) != 0) {
		/* Yes, return the size */
		return node->size - 1;
	}

	/* Get current size */
	size_t nodesize = node->size;

	/* Compute pointer past the last valid node */
	struct static_node *end =
		(struct static_node*) (map->buffer + map->size);

	/* Construct pointer to next node */
	struct static_node *next =
		(struct static_node*) (((char*) node) + nodesize);
	assert(next <= end);

	/* Try to merge successive free nodes into current node */
	while (next != end && (next->size & 1u) == 0) {
		/* Merge next node to the current node */
		assert(next->size >= sizeof(struct static_node));
		nodesize += next->size;

		/* Construct pointer past the merged node */
		next = (struct static_node*)
			(((char*) node) + nodesize);
		assert(next <= end);
	}

	/* Store new size to current node */
	node->size = nodesize;

	/* Make sure that start always points to a valid node */
	if (node < map->start && map->start < next) {
		if (next != (struct static_node*) (map->buffer + map->size))
			map->start = next;
		else
			map->start = (struct static_node*) map->buffer;
	}

	/* Return the new size */
	return nodesize;
}


/* Get next node */
static struct static_node *get_successor(
	struct static_allocator *map, struct static_node *node)
{
	/* Node must reside in buffer */
	assert((struct static_node*) map->buffer <= node);
	assert(node + 1 <= (struct static_node*) (map->buffer + map->size));

	/* Node size must be valid */
	assert((node->size & ~1u) >= sizeof(struct static_node));
	assert(((char*)node) + (node->size & ~1u) <= map->buffer + map->size);

	/* Compute address of the next node */
	struct static_node *next =
		(struct static_node*) (((char*) node) + (node->size & ~1u));

	/* If this is the last node, then restart from beginning */
	if (next == (struct static_node*) (map->buffer + map->size))
		next = (struct static_node*) map->buffer;

	return next;
}


/* Allocate bytes from node */
static void *allocate_node(
	struct static_allocator *map, struct static_node *node,
	size_t new_size)
{
	/* Node must reside in buffer */
	assert((struct static_node*) map->buffer <= node);
	assert(node + 1 < (struct static_node*) (map->buffer + map->size));

	/* Node must be free */
	assert((node->size & 1u) == 0);

	/* Size must be valid */
	assert(node->size >= sizeof(struct static_node));
	assert(((char*) node) + node->size <= map->buffer + map->size);

	/* Node must accommodiate requested size */
	assert(new_size >= sizeof(struct static_node));
	assert(new_size <= node->size);

	/* Get current size of node */
	size_t nodesize = node->size;

	/* Do we have excess room at the end of node? */
	if (new_size < nodesize) {
		/*
		 * Yes, split the node to avoid wasting space.
		 *
		 * Note that the code below can create a node with no room for
		 * custom data, i.e. node size will be equal to the size of
		 * static_node_t.  This is intentional and helps to maximize
		 * the amount of space available: the empty node may be merged
		 * with its successor later and the space may be reclaimed.
		 */
		node->size = (new_size | 1);

		/* Compute pointer to free space after the node */
		struct static_node *next =
			(struct static_node*) (((char*) node) + new_size);
		assert(next < (struct static_node*) (map->buffer + map->size));

		/* Create new node */
		next->size = nodesize - new_size;
		assert(next->size >= sizeof(struct static_node));
		assert(((char*)next) + next->size <= map->buffer + map->size);
	} else {
		/* Mark the whole node allocated */
		node->size = (nodesize | 1);
	}

	/*
	 * Start the next search from this node when we need more memory.
	 * Note that setting the start variable also ensures that the variable
	 * points to a valid node even though we combine nodes above.
	 */
	map->start = node;

	/* Return the start of free memory area within node */
	return (void*) &node[1];
}


/* Move node to another area */
static void *relocate_node(
	struct allocator *ap, struct static_node *node, size_t n)
{
	/* Compute the size of payload in current node */
	size_t size = (node->size & ~1u) - sizeof(struct static_node);
	assert(size <= n);

	/* Compute the starting address of the payload */
	void *p = &node[1];

	/* Allocate a fresh memory area for the enlarged data */
	void *q = ap->vtable->grab(ap, n);
	if (!q)
		return NULL;

	/* Copy memory from old area to the new area */
	copy_memory(q, p, size);

	/* Release the old memory area */
	ap->vtable->release(ap, p);

	/* Return pointer to the new memory area */
	return q;
}


/* Returns true if node is free */
static int is_free(struct static_node *node)
{
	return (node->size & 1u) == 0;
}
