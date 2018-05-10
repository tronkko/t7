/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/thread.h"
#include "t7/tls.h"
#include "t7/memory.h"
#include "t7/critical-section.h"

#undef NDEBUG
#include <assert.h>


/* Local functions */
static tls_variable_t *allocate_dummy (void);
static void free_dummy (tls_variable_t *vp);
static int create_dummy (tls_variable_t *vp, const tls_type_t *tp);
static void destroy_dummy (tls_variable_t *vp);
static void *get_dummy (tls_variable_t *vp);

static tls_variable_t *allocate_dynamic (void);
static void free_dynamic (tls_variable_t *vp);
static int create_dynamic (tls_variable_t *vp, const tls_type_t *tp);
static void destroy_dynamic (tls_variable_t *vp);
static void *get_dynamic (tls_variable_t *vp);

static int my_main (thread_t *tp);

static void test_single (void);
static void test_threads (void);


/* Custom TLS variable type */
struct dummy_tls_variable {
    tls_variable_t base;
    int value;
};

/* Another custom TLS variable type */
struct dynamic_tls_variable {
    tls_variable_t base;
    char *buffer;
};

/* Thread-local variable 1 */
static tls_type_t mytp1 = {
    allocate_dummy,
    free_dummy,
    create_dummy,
    destroy_dummy,
    get_dummy,
};

/* Thread-local variable 2 */
static tls_type_t mytp2 = {
    allocate_dummy,
    free_dummy,
    create_dummy,
    destroy_dummy,
    get_dummy,
};

/* Thread-local variable 3 */
static tls_type_t mytp3 = {
    allocate_dynamic,
    free_dynamic,
    create_dynamic,
    destroy_dynamic,
    get_dynamic,
};

/* Custom thread */
static thread_type_t def1 = {
    allocate_thread,
    free_thread,
    create_thread,
    destroy_thread,
    my_main,
};
static thread_type_t *my_thread = &def1;


int
main (void)
{
    test_single ();
    if (has_threads ()) {
        test_threads ();
    }
    return 0;
}


/* Make sure that TLS variables work on a single thread (also main thread) */
static void
test_single (void)
{
    int *p;
    char *pc;
    size_t i;
    size_t j;

    /* Register tls variable with value of zero */
    p = get_tls (&mytp1);
    assert (p != NULL);
    assert (*p == 0);

    /* Set tls variable to 13 */
    *p = 13;

    /* Get tls variable again */
    p = get_tls (&mytp1);
    assert (p != NULL);
    assert (*p == 13);

    /* Register another tls variable */
    p = get_tls (&mytp2);
    assert (p != NULL);
    assert (*p == 0);

    /* Set second variable to 666 */
    *p = 666;

    /* Value of first variable is still 13 */
    p = get_tls (&mytp1);
    assert (p != NULL);
    assert (*p == 13);

    /* Value of second variable is still 666 */
    p = get_tls (&mytp2);
    assert (p != NULL);
    assert (*p == 666);

    /* Register complex variable */
    pc = get_tls (&mytp3);
    assert (pc != NULL);
    assert (*pc == '\0');

    /* Save some data to complex variable */
    fill_memory (pc, 'x', 1000);

    /* Value of second variable is still 666 */
    p = get_tls (&mytp2);
    assert (p != NULL);
    assert (*p == 666);

    /* Value of complex variable is intact */
    pc = get_tls (&mytp3);
    assert (pc != NULL);
    for (i = 0; i < 1000; i++) {
        assert (pc[i] == 'x');
    }
    assert (pc[1000] == '\0');

    /* Let other threads run and make sure that variables stay intact */
    for (j = 0; j < 10000; j++) {

        /* Value of first variable is still 13 */
        p = get_tls (&mytp1);
        assert (p != NULL);
        assert (*p == 13);

        /* Value of second variable is still 666 */
        p = get_tls (&mytp2);
        assert (p != NULL);
        assert (*p == 666);

        /* Value of complex variable is intact */
        pc = get_tls (&mytp3);
        assert (pc != NULL);
        for (i = 0; i < 1000; i++) {
            assert (pc[i] == 'x');
        }
        assert (pc[1000] == '\0');

    }
}


/* Create several threads and make sure that threads are independent */
static void
test_threads (void)
{
    thread_t *tp[10];
    int ok;
    size_t i;

    /* Create ten threads */
    for (i = 0; i < 10; i++) {
        tp[i] = new_thread (my_thread);
        assert (tp[i] != NULL);
    }

    /* Start threads */
    for (i = 0; i < 10; i++) {
        ok = start_thread (tp[i]);
        assert (ok);
    }

    /* Wait for threads to complete */
    for (i = 0; i < 10; i++) {
        ok = join_thread (tp[i]);
        assert (ok);
        delete_thread (tp[i]);
    }
}


static int
my_main (thread_t *tp)
{
    /* Ignore parameter */
    (void) tp;

    /* Execute single-threaded test */
    test_single ();
    return 1;
}


/* Allocate memory for simple TLS variable */
static tls_variable_t *
allocate_dummy (void)
{
    return (tls_variable_t*)
        allocate_memory (sizeof (struct dummy_tls_variable));
}


/* Release dummy TLS variable */
static void
free_dummy (tls_variable_t *vp)
{
    free_memory (vp);
}


/* Initialize dummy TLS variable */
static int
create_dummy (tls_variable_t *vp, const tls_type_t *tp)
{
    int ok;

    /* Initialize base variable */
    if (create_tls (vp, tp)) {

        /* Initialize custom data */
        struct dummy_tls_variable *dp = (struct dummy_tls_variable*) vp;
        dp->value = 0;

        /* Success */
        ok = 1;

    } else {

        /* Cannot initialize base variable */
        ok = 0;

    }
    return ok;
}


/* Un-initialize dummy TLS variable */
static void
destroy_dummy (tls_variable_t *vp)
{
    /* Destroy base variable */
    destroy_tls (vp);
}


/* Get address of custom data */
static void *
get_dummy (tls_variable_t *vp)
{
    struct dummy_tls_variable *dp = (struct dummy_tls_variable*) vp;
    return &dp->value;
}


/* Allocate memory for complex TLS variable */
static tls_variable_t *
allocate_dynamic (void)
{
    return allocate_memory (sizeof (struct dynamic_tls_variable));
}


/* Release complex TLS variable */
static void
free_dynamic (tls_variable_t *vp)
{
    free_memory (vp);
}


/* Initialize complex TLS variable */
static int
create_dynamic (tls_variable_t *vp, const tls_type_t *tp)
{
    struct dynamic_tls_variable *dp;
    char *p;
    int ok;

    /* Initialize base variable */
    if (create_tls (vp, tp)) {

        /* Allocate memory for buffer */
        p = allocate_memory (1024);
        if (p) {

            /* Zero-fill buffer */
            zero_memory (p, 1024);

            /* Save pointer to structure */
            dp = (struct dynamic_tls_variable*) vp;
            dp->buffer = p;

            /* Success */
            ok = 1;

        } else {

            /* Failure */
            ok = 0;

        }

    } else {

        /* Failed to initialize base variable */
        ok = 0;

    }
    return ok;
}


/* Un-initialize custom data */
static void
destroy_dynamic (tls_variable_t *vp)
{
    struct dynamic_tls_variable *dp = (struct dynamic_tls_variable*) vp;
    assert (dp != NULL);

    /* Release buffer */
    assert (dp->buffer != NULL);
    free_memory (dp->buffer);
    dp->buffer = NULL;

    /* Destroy base variable */
    destroy_tls (vp);
}


/* Get pointer to complex TLS data */
static void *
get_dynamic (tls_variable_t *vp)
{
    struct dynamic_tls_variable *dp = (struct dynamic_tls_variable*) vp;
    return dp->buffer;
}

