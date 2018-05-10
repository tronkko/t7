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


/* Private allocator functions */
static int create (allocator_t *ap, const allocator_type_t *tp);
static void destroy (allocator_t *ap);

/* Define static allocator type */
static allocator_type_t alloc1 = {
    allocate_static_allocator,
    free_static_allocator,
    create,
    destroy,
    static_grab_memory,
    static_release_memory,
    static_resize_memory
};
const allocator_type_t *static_allocator = &alloc1;


/* Allocate room for static allocator object */
allocator_t *
allocate_static_allocator (void)
{
    return system_allocate_memory (sizeof (static_allocator_t));
}


/* Release static allocator object */
void
free_static_allocator (allocator_t *ap)
{
    system_free_memory (ap);
}


/* Initialize static allocator with custom buffer */
int
create_static_allocator(
    allocator_t *ap, const allocator_type_t *tp, char *buffer, size_t size)
{
    static_allocator_t *map;
    static_node_t *node;
    int ok;

    /* Buffer must be provided */
    assert (buffer != NULL);

    /* Buffer must be aligned to 4 or 8-byte boundary */
    assert ((((size_t) buffer) & (sizeof (static_node_t) - 1)) == 0);

    /* Size must be multiple of 16 to ensure that all nodes are valid */
    assert ((size & 0xf) == 0);

    /* Initialize standard fields */
    if (create_allocator (ap, tp)) {

        /* Convert allocator to static allocator */
        map = (static_allocator_t*) ap;
        assert (&map->base == ap);

        /* Save buffer data */
        map->buffer = buffer;
        map->size = size;

        /* Reset memory buffer (for debugging) */
#ifndef NDEBUG
        fill_memory (buffer, 0xCC, size);
#endif

        /* Create a free memory node at the beginning of the buffer */
        node = (static_node_t*) buffer;
        node->size = size;

        /* Start with the initialized node */
        map->start = node;

        /* Return success */
        ok = 1;

    } else {

        /* Failed to initialize default allocator */
        ok = 0;

    }
    return ok;
}


/* Initialize default static allocator */
static int
create (allocator_t *ap, const allocator_type_t *tp)
{
    size_t size;
    int ok;
    char *buffer;

    /* Pick a suitable size for buffer */
    size = 1024 * 1024;

    /* Allocate room for buffer */
    buffer = system_allocate_memory (size);
    if (buffer) {

        /* Initialize with dynamic buffer */
        ok = create_static_allocator (ap, tp, buffer, size);

    } else {

        /* Not enough memory to allocate buffer */
        ok = 0;

    }
    return ok;
}


/* Un-initialize default static allocator */
static void
destroy (allocator_t *ap)
{
    static_allocator_t *map;

    /* Convert allocator to static allocator */
    map = (static_allocator_t*) ap;

    /* Release buffer */
    if (map->buffer) {

        /* Reset memory buffer (for debugging) */
#ifndef NDEBUG
        fill_memory (map->buffer, 0xFF, map->size);
#endif

        /* Release buffer */
        system_free_memory (map->buffer);

    }

    /* Reset fields */
#ifndef NDEBUG
    map->start = (static_node_t*) -1;
    map->buffer = (char*) -1;
    map->size = (size_t) -1;
#endif
}


/* Allocate memory from static allocator */
void *
static_grab_memory (allocator_t *ap, size_t n)
{
    static_allocator_t *map;
    size_t alloc_bytes;
    static_node_t *node;
    static_node_t *first;
    static_node_t *end;
    int count;
    void *result = NULL;

    /* Lock out other threads */
    enter_critical ();

    /*
     * Round up the size to ensure proper alignment of data types and to
     * preserve space for static node.
     *
     * For example, if the caller is requesting one byte of memory, we need to
     * allocate at least 8 bytes in order to align structures on a 32-bit
     * architecture.
     */
    if (n < sizeof (static_node_t)) {
        /* Align to 4-byte (32-bit) or 8-byte boundary (64-bit) */
        alloc_bytes = sizeof (static_node_t) * 2;
    } else {
        /* Align to 8-byte boundary */
        alloc_bytes = sizeof (static_node_t) + ((n + 7u) & ~7u);
    }
    assert (alloc_bytes >= n);
    assert (alloc_bytes > sizeof (static_node_t));
    assert ((alloc_bytes & 1) == 0);

    /* Convert allocator to static allocator */
    map = (static_allocator_t*) ap;

    /* Compute address of first valid memory node */
    first = (static_node_t*) map->buffer;

    /* Compute pointer past the last valid memory node */
    end = (static_node_t*) (map->buffer + map->size);

    /* Get pointer to current memory node */
    node = map->start;
    assert (node != NULL);
    assert (first <= node  &&  node < end);

    /*
     * Loop through memory nodes and pick the first free memory node that
     * is large enough to satisfy the request.
     */
    count = 0;
    while (1) {
        static_node_t *next;
        size_t nodesize;
        int status;

        /*
         * Get size and allocation status of node pointed by variable node.
         * Note that size and allocation status are packed into single size_t
         * variable.  The least significant bit is set if node is allocated
         * and zero otherwise.  For example, number 33 refers to an allocated
         * 32-byte memory region whereas 32 refers to a free 32-byte memory
         * region.
         */
        nodesize = (node->size & ~1u);
        status = (node->size & 1u);

        /* Construct pointer to next node */
        next = (static_node_t*) (((char*) node) + nodesize);

        /* Merge successive free nodes */
        while (status == 0  &&  next != end  &&  (next->size & 1u) == 0) {

            /* Merge next node to the current node */
            nodesize += next->size;

            /* Construct pointer past the merged node */
            next = (static_node_t*) (((char*) node) + nodesize);

        }

        /* Does the node satisfy the request? */
        if (status == 0  &&  alloc_bytes <= nodesize) {

            /* Yes, compute pointer to free space after the node */
            next = (static_node_t*) (((char*) node) + alloc_bytes);

            /* Do we have excess room at the end of node? */
            if (next != end  &&  alloc_bytes < nodesize) {

                /*
                 * Yes, split the node to avoid wasting space
                 *
                 * Note that the code below can create a node with no room for
                 * custom data, i.e. node size will be equal to the size of
                 * static_node_t.  This is intentional and helps to maximize
                 * the amount of space available: the empty node may be merged
                 * with its successor later and the space may be reclaimed.
                 */
                node->size = (alloc_bytes | 1);
                next->size = nodesize - alloc_bytes;
                assert (next->size >= sizeof (static_node_t));

            } else {

                /* Mark the whole node allocated */
                node->size = (nodesize | 1);

            }

            /* Start the next search from this node */
            map->start = node;

            /* Return the memory area to caller */
            result = (void*) &node[1];
            break;

        }

        /* Do we have more nodes in the table? */
        if (next != end) {

            /* Yes, continue with the successive node */
            node = next;

        } else {

            /* No, we have reached the end of table */
            if (count == 0) {

                /* Continue from the beginning of the table */
                node = first;
                count++;

            } else {

                /*
                 * We have looped through the table once without finding a
                 * free node!
                 *
                 * Note that we enter here only when the end of table is
                 * reached for the second time.  This is a simple but
                 * ineffective way to ensure that the loop does end at some
                 * point.  The ineffectiveness stems from the fact that if
                 * map->start points to a memory node at the beginning of the
                 * table and there is not enough memory to satisfy the
                 * request, then we need to process almost every memory node
                 * twice in order to reach this conclusion.
                 *
                 * If you were to optimize the code, then you might consider
                 * stopping the search when the node pointed by map->start is
                 * found for the second time.  However, for that to work, you
                 * would have to be very careful when merging nodes: if
                 * map->start points to a node who is merged with the previous
                 * node at the end of the first round, then the node pointer
                 * may never be equal to map->start which leads to an infinite
                 * loop.
                 *
                 * Personally I am not worried about the inefficiency since
                 * there is usually more than enough memory!
                 */
                result = NULL;
                break;

            }

        }

    }

    leave_critical ();
    return result;
}


/* Release memory back to static allocator */
void
static_release_memory (allocator_t *ap, void *p)
{
    static_allocator_t *map;
    static_node_t *node;

    /* Preconditions */
    assert (p != NULL);

    /* Lock out other threads */
    enter_critical ();

    /* Convert allocator to static allocator */
    map = (static_allocator_t*) ap;

    /* Construct pointer to memory node */
    node = &((static_node_t*) p)[-1];

    /* Ensure that memory was allocated using this allocator */
    assert ((static_node_t*) map->buffer <= node);
    assert (node < (static_node_t*) (map->buffer + map->size));

    /* Make sure that memory is allocated (avoid double free) */
    assert ((node->size & 1) != 0);

    /* Reset first bit to release the memory */
    node->size = (node->size & ~1u);

    /* Reset memory area (for debugging) */
#ifndef NDEBUG
    assert (map->buffer < (char*) p);
    assert (((char*) p) + node->size - sizeof (static_node_t) <= map->buffer + map->size);
    fill_memory (p, 0xFF, node->size - sizeof (static_node_t));
#endif

    leave_critical ();
}


/* Resize memory region */
void *
static_resize_memory (allocator_t *ap, void *p, size_t n)
{
    void *result;
    static_allocator_t *map;
    static_node_t *node;
    static_node_t *next;
    static_node_t *end;
    size_t available;
    size_t alloc_bytes;

    /* Preconditions */
    assert (p != NULL);
    assert (n != 0);

    /* Round the size up to ensure proper alignment of data types */
    if (n < sizeof (static_node_t)) {
        alloc_bytes = sizeof (static_node_t) * 2;
    } else {
        alloc_bytes = sizeof (static_node_t) + ((n + 7u) & ~7u);
    }
    assert (alloc_bytes >= n);
    assert (alloc_bytes > sizeof (static_node_t));
    assert ((alloc_bytes & 1) == 0);

    /* Convert pointer to static allocator */
    map = (static_allocator_t*) ap;

    /* Construct pointer to memory node in question */
    node = &((static_node_t*) p)[-1];

    /* Ensure that memory was allocated using this allocator */
    assert ((static_node_t*) map->buffer <= node);
    assert (node < (static_node_t*) (map->buffer + map->size));

    /* Avoid handling already freed memory */
    assert ((node->size & 1) != 0);

    /* Lock out other threads */
    enter_critical ();

    /* Compute pointer past the last valid memory node */
    end = (static_node_t*) (map->buffer + map->size);

    /* Get size of this node (including header) */
    available = (node->size & ~1u);

    /* Construct pointer to next node */
    next = (static_node_t*) (((char*) node) + available);

    /* Compute the size of this node plus successive free nodes */
    while (next != end  &&  (next->size & 1u) == 0) {

        /* Include successive node in size */
        available += next->size;

        /* Construct pointer to next node */
        next = (static_node_t*) (((char*) node) + available);

    }

    /* Is the combined memory area large enough? */
    if (alloc_bytes <= available) {

        /* Yes, compute pointer to free space after the node */
        next = (static_node_t*) (((char*) node) + alloc_bytes);

        /* Does the combined memory area leave some bytes free? */
        if (next != end  &&  alloc_bytes < available) {

            /*
             * Yes, split the node to avoid wasting space.
             *
             * Note that the code below can create a node with no room for
             * custom data, i.e. node size will be equal to the size of
             * static_node_t.  This is intentional and helps to maximize the
             * amount of space available: the empty node may be merged with
             * its successor later and the space may be reclaimed.
             */
            node->size = (alloc_bytes | 1);
            next->size = available - alloc_bytes;
            assert (next->size >= sizeof (static_node_t));

        } else {

            /* Mark the whole node as allocated */
            node->size = (available | 1);

        }

        /* Start the next search from this node */
        map->start = node;

        /* Return the memory area to caller */
        result = (void*) &node[1];

    } else {

        /*
         * The combined memory node cannot satisfy the request and we must
         * relocate the memory area to another address.
         */
        void *q;
        size_t copy_bytes;

        /* Compute size of memory region pointed by p */
        copy_bytes = (node->size & ~1u) - sizeof (static_node_t);
        assert (copy_bytes <= n);

        /* Allocate a fresh memory area for the enlarged data */
        q = ap->type->grab (ap, n);
        if (q) {

            /* Copy memory from old area to the new area */
            copy_memory (q, p, copy_bytes);

            /* Return the new memory area */
            result = q;

            /* Release old memory area */
            ap->type->release (ap, p);

        } else {

            /* Could not allocate another memory area */
            result = NULL;

        }

    }

    leave_critical ();
    return result;
}


