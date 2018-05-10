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


/* Allocator functions */
static allocator_t *allocate_faulty_allocator (void);
static void free_faulty_allocator (allocator_t *ap);
static void *faulty_allocate_memory (allocator_t *ap, size_t n);
static void faulty_free_memory (allocator_t *ap, void *p);
static void *faulty_resize_memory (allocator_t *ap, void *p, size_t n);

/* Allocator type */
static allocator_type_t def1 = {
    allocate_faulty_allocator,
    free_faulty_allocator,
    create_allocator,
    destroy_allocator,
    faulty_allocate_memory,
    faulty_free_memory,
    faulty_resize_memory,
};
const allocator_type_t *faulty_allocator = &def1;

/* Static allocator object */
static allocator_t singleton;

/* Default fixture for testing */
static fixture_t def2 = {
    get_faulty_allocator
};
fixture_t *test_fixture = &def2;


/* Get allocator for test_fixture */
static allocator_t*
get_faulty_allocator (fixture_t *fp)
{
    (void) fp;
    return get_allocator (faulty_allocator);
}


/* Allocate allocator structure */
static allocator_t *
allocate_faulty_allocator (void)
{
    /* Return reference to single instance */
    return &singleton;
}


/* Release allocator */
static void
free_faulty_allocator (allocator_t *ap)
{
    (void) ap;
    /*NOP*/;
}


/* Allocate memory from faulty allocator */
static void*
faulty_allocate_memory (allocator_t *ap, size_t n)
{
    void *p;
    (void) ap;

    if (!simulate_failure ()) {

        /* No simulation, allocate memory from system */
        p = system_allocate_memory (n);

    } else {

        /* Simulated failure */
        p = NULL;

    }
    return p;
}


/* Release memory back to faulty allocator */
static void
faulty_free_memory (allocator_t *ap, void *p)
{
    (void) ap;

    /* Return memory directly to system */
    system_free_memory (p);
}


/* Resize memory region */
static void*
faulty_resize_memory (allocator_t *ap, void *p, size_t n)
{
    void *q;
    (void) ap;

    if (!simulate_failure ()) {

        /* No simulation, allocate memory from system */
        q = system_resize_memory (p, n);

    } else {

        /* Simulated failure */
        q = NULL;

    }
    return q;
}


