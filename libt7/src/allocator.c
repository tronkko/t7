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


/* Get instance of an allocator */
static allocator_t *find_allocator (const allocator_type_t *tp);

/* Default heap allocator */
static void *default_allocate_memory (allocator_t *ap, size_t n);
static void default_free_memory (allocator_t *ap, void *p);
static void *default_resize_memory (allocator_t *ap, void *p, size_t n);

/* Cleanup function */
static void cleanup (void);


/* Default allocator type */
static allocator_type_t def = {
    allocate_allocator,
    free_allocator,
    create_allocator,
    destroy_allocator,
    default_allocate_memory,
    default_free_memory,
    default_resize_memory,
};
const allocator_type_t *default_allocator = &def;

/* List of allocators */
static allocator_t head;
static allocator_t tail;

/* True if allocator list initialized */
static int initialized = 0;


/* Get reference to default allocator */
allocator_t *
get_default_allocator (void)
{
    fixture_t *fp;
    allocator_t *ap;

    /* Get current fixture (will terminate app on failure) */
    fp = get_fixture ();
    assert (fp != NULL);

    /* Retrieve allocator from fixture */
    assert (fp->get_fixture_allocator != NULL);
    ap = fp->get_fixture_allocator (fp);

    return ap;
}


/* Get reference to given allocator */
allocator_t *
get_allocator (const allocator_type_t *tp)
{
    allocator_t *ap;
    enter_critical ();

    /* Is the module initialized? */
    if (initialized) {

        /* Yes, find/create allocator */
        ap = find_allocator (tp);

    } else {

        /* No, register exit function to clean up variables */
        if (exit_handler (cleanup, 20)) {

            /* Initialize head node */
            head.next = &tail;
            head.prev = NULL;
            head.type = NULL;

            /* Initialize tail node */
            tail.next = NULL;
            tail.prev = &head;
            tail.type = NULL;

            /* Mark initialization as done */
            initialized = 1;

            /* Create first allocator */
            ap = find_allocator (tp);

        } else {

            /* Cannot install exit function */
            ap = NULL;

        }
    }

    leave_critical ();
    return ap;
}


/* Find allocator by type */
static allocator_t *
find_allocator (const allocator_type_t *tp)
{
    allocator_t *ap;
    int found = 0;

    /* Loop through allocators in list */
    ap = head.next;
    while (ap->next != NULL) {

        /* Is this the allocator we are looking for? */
        if (ap->type != tp) {
            /* No, check next allocator in list */
            ap = ap->next;
        } else {
            /* Yes */
            found = 1;
            break;
        }

    }

    /* Create new allocator if needed */
    if (!found) {
        ap = new_allocator (tp);
        if (ap) {

            /* Add allocator to end of list */
            ap->next = &tail;
            ap->prev = tail.prev;
            tail.prev->next = ap;
            tail.prev = ap;

        } else {

            /* Allocation failed */
            ap = NULL;

        }
    }
    return ap;
}


/* Create new allocator */
allocator_t *
new_allocator (const allocator_type_t *tp)
{
    allocator_t *ap;

    /* Pre-conditions */
    assert (tp != NULL);
    assert (tp->allocate != NULL);
    assert (tp->free != NULL);
    assert (tp->create != NULL);
    assert (tp->destroy != NULL);
    assert (tp->grab != NULL);
    assert (tp->release != NULL);
    assert (tp->resize != NULL);

    /* Allocate memory for allocator */
    ap = tp->allocate ();
    if (ap) {

        /* Initialize custom attributes */
        if (tp->create (ap, tp)) {

            /* Success */
            assert (ap->type == tp);

        } else {

            /* Initialization failed */
            tp->free (ap);
            ap = NULL;

        }

    } else {

        /* Out of memory? */
        ap = NULL;

    }
    return ap;
}


/* Delete allocator with resources */
void
delete_allocator (allocator_t *ap)
{
    if (ap) {
        const allocator_type_t *tp = ap->type;

        /* Pre-conditions */
        assert (tp != NULL);
        assert (tp->free != NULL);
        assert (tp->destroy != NULL);

        /* Detach allocator from list */
        if (ap->next) {
            assert (ap->next->prev == ap);
            ap->next->prev = ap->prev;
        }
        if (ap->prev) {
            assert (ap->prev->next == ap);
            ap->prev->next = ap->next;
        }

        /* Destroy custom allocator */
        tp->destroy (ap);

        /* Release allocator object */
        tp->free (ap);

    }
}


/* Allocate memory from allocator */
void *
allocator_allocate_memory (allocator_t *ap, size_t n)
{
    void *p;

    /* Pre-conditions */
    assert (ap != NULL);

    /* Is real allocation request? */
    if (n) {

        /* Yes, allocate memory through allocator */
        assert (ap->type->grab != NULL);
        p = ap->type->grab (ap, n);

    } else {

        /* No, return null pointer */
        p = NULL;

    }

    return p;
}


/* Resize memory using allocator */
void *
allocator_resize_memory (allocator_t *ap, void *p, size_t n)
{
    void *q;

    /* Pre-conditions */
    assert (ap != NULL);

    /* Do we have a memory area to resize? */
    if (p) {

        /* Yes, is the new size greater than zero? */
        if (n) {

            /* Yes, resize memory through allocator */
            assert (ap->type->resize != NULL);
            q = ap->type->resize (ap, p, n);

        } else {

            /* Resizing to zero bytes => release memory area */
            assert (ap->type->release != NULL);
            ap->type->release (ap, p);
            q = NULL;

        }

    } else {

        if (n) {

            /* Resizing null pointer => allocate fresh memory area */
            assert (ap->type->grab != NULL);
            q = ap->type->grab (ap, n);

        } else {

            /* Resizing NULL pointer to zero bytes?!? */
            q = NULL;

        }

    }

    return q;
}


/* Release memory using allocator */
void
allocator_free_memory (allocator_t *ap, void *p)
{
    /* Pre-conditions */
    assert (ap != NULL);

    /* Is memory allocated? */
    if (p) {

        /* Yes, release memory through allocator */
        assert (ap->type->release != NULL);
        ap->type->release (ap, p);

    } else {

        /* No, freeing a NULL pointer */
        /*NOP*/;

    }
}


/* Allocate memory for default allocator */
allocator_t *
allocate_allocator (void)
{
    return (allocator_t*) system_allocate_memory (sizeof (allocator_t));
}


/* Release default allocator */
void
free_allocator (allocator_t *ap)
{
    system_free_memory (ap);
}


/* Initialize default allocator */
int
create_allocator (allocator_t *ap, const allocator_type_t *tp)
{
    ap->next = NULL;
    ap->prev = NULL;
    ap->type = tp;
    return 1;
}


/* Un-initialize default allocator */
void
destroy_allocator (allocator_t *ap)
{
#ifndef NDEBUG
    /* Reset fields */
    ap->next = (allocator_t*) -1;
    ap->prev = (allocator_t*) -1;
    ap->type = (allocator_type_t*) -1;
#else
    /* Silence warnings about unused argument */
    (void) ap;
#endif
}


/* Allocate memory from default allocator */
static void *
default_allocate_memory (allocator_t *ap, size_t n)
{
    (void) ap;
    return system_allocate_memory (n);
}


/* Release memory from default allocator */
static void
default_free_memory (allocator_t *ap, void *p)
{
    (void) ap;
    system_free_memory (p);
}


/* Resize memory region */
static void *
default_resize_memory (allocator_t *ap, void *p, size_t n)
{
    (void) ap;
    return system_resize_memory (p, n);
}


/* Release allocators at program exit */
static void
cleanup (void)
{
    allocator_t *ap;

    /* Preconditions */
    assert (initialized);

    /*
     * Loop through allocators and destroy them.
     *
     * Be ware that the code below assumes that no other threads are running.
     * If some threads are still running, then they might be using the
     * allocator being destroyed which causes the program to crash!
     *
     * Also note that the code below assumes that allocators are mostly
     * independent of each other.  If an allocator calls another allocator to
     * do its bidding, then the code below will crash due if the underlying
     * allocator has already been destroyed!  In order to alleviate this
     * problem, we destroy allocators in the reverse order.
     */
    ap = tail.prev;
    while (ap->prev != NULL) {
        allocator_t *prev;

        /* Grab pointer to previous allocator */
        prev = ap->prev;

        /* Destroy allocator and detach it from list */
        delete_allocator (ap);

        /* Continue with previous allocator */
        ap = prev;
    }

    /* Ensure that the list is empty */
    assert (head.next == &tail);
    assert (tail.prev == &head);
}

