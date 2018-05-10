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
static allocator_t *allocate_my_allocator (void);
static void free_my_allocator (allocator_t *ap);
static int create_my_allocator (allocator_t *ap, const allocator_type_t *tp);
static void destroy_my_allocator (allocator_t *ap);

/* Custom allocator type */
static allocator_type_t def = {
    allocate_my_allocator,
    free_my_allocator,
    create_my_allocator,
    destroy_my_allocator,
    static_grab_memory,
    static_release_memory,
    static_resize_memory,
};
static const allocator_type_t *my_allocator = &def;

/* The single allocator instance */
static static_allocator_t singleton;

/* Static buffer */
static char buffer[1024];


int
main (void)
{
    allocator_t *ap;
    char *p1;
    char *p2;
    char *ptrs[64];
    size_t i;

    /* Create allocator */
    ap = get_allocator (my_allocator);
    assert (ap != NULL);

    /* Allocate 1000 bytes */
    p1 = allocator_allocate_memory (ap, 1000);
    assert (p1 != NULL);

    /* Allocation of another 1000 bytes cannot succeed */
    p2 = allocator_allocate_memory (ap, 1000);
    assert (p2 == NULL);

    /* Reduce size to 100 bytes ... */
    p1 = allocator_resize_memory (ap, p1, 100);
    assert (p1 != NULL);

    /* ... after which there is room to allocate 800 bytes */
    p2 = allocator_allocate_memory (ap, 800);
    assert (p2 != NULL);

    /* Release both memory areas */
    allocator_free_memory (ap, p1);
    allocator_free_memory (ap, p2);

    /* Buffer of 1024 bytes can accommodiate 64 integers on a 64-bit system */
    for (i = 0; i < 64; i++) {
        ptrs[i] = allocator_allocate_memory (ap, sizeof (int));
        if (ptrs[i] == NULL) {
            assert (ptrs[i] != NULL);
        }
    }

    /* Release integers */
    for (i = 0; i < 64; i++) {
        allocator_free_memory (ap, ptrs[i]);
    }

    return 0;
}


/* Allocate room for static allocator structure */
static allocator_t*
allocate_my_allocator (void)
{
    /* Return pointer to sole allocator instance */
    return (allocator_t*) &singleton;
}


/* Release static allocator */
static void
free_my_allocator (allocator_t *ap)
{
    (void) ap;
    /*NOP*/;
}


/* Initialize allocator */
static int
create_my_allocator (allocator_t *ap, const allocator_type_t *tp)
{
    /* Initialize static allocator using statically allocated buffer */
    return create_static_allocator (ap, tp, buffer, sizeof (buffer));
}


/* Uninitialize allocator */
static void
destroy_my_allocator (allocator_t *ap)
{
    destroy_allocator (ap);
}


