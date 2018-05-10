/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/thread.h"
#include "t7/critical-section.h"

#undef NDEBUG
#include <assert.h>


/* Local functions */
static int increments (thread_t *tp);
static void test_increments (void);
static int paddle_odd (thread_t *tp);
static int paddle_even (thread_t *tp);
static void test_paddling (void);

/* Define thread types */
static thread_type_t def1 = {
    allocate_thread,
    free_thread,
    create_thread,
    destroy_thread,
    increments
};
static thread_type_t *increment_thread = &def1;

static thread_type_t def2 = {
    allocate_thread,
    free_thread,
    create_thread,
    destroy_thread,
    paddle_odd
};
static thread_type_t *odd_thread = &def2;

static thread_type_t def3 = {
    allocate_thread,
    free_thread,
    create_thread,
    destroy_thread,
    paddle_even
};
static thread_type_t *even_thread = &def3;


/* Plain variable for testing threads */
static volatile int counter;


int
main (void)
{
    test_increments ();
    if (has_threads ()) {
        test_paddling ();
    }
    return 0;
}


/* Increment variable by multiple threads */
static void
test_increments (void)
{
    thread_t *tp[10];
    size_t i;
    size_t j;
    int result;
    int check[100*10];

    /* Reset counter */
    counter = 0;

    /* Reset results array */
    for (i = 0; i < 100*10; i++) {
        check[i] = 0;
    }

    /* Repeat the test 100 times */
    for (i = 0; i < 100; i++) {

        /* Create 10 concurrent threads */
        for (j = 0; j < 10; j++) {
            tp[j] = new_thread (increment_thread);
            assert (tp[j] != NULL);
            result = start_thread (tp[j]);
            assert (result != 0);
        }

        /* Wait for every thread to complete */
        for (j = 0; j < 10; j++) {
            result = join_thread (tp[j]);
            assert (0 <= result  &&  result < 100*10);
            check[result]++;
            delete_thread (tp[j]);
        }

    }

    /* Make sure that counter passes through all values */
    for (i = 0; i < 100*10; i++) {
        assert (check[i] == 1);
    }
}


/* Increment variable by switching between two threads */
static void
test_paddling (void)
{
    thread_t *t1;
    thread_t *t2;
    int result;

    /* Reset counter */
    counter = 0;

    /* Create two threads */
    t1 = new_thread (even_thread);
    assert (t1 != NULL);
    t2 = new_thread (odd_thread);
    assert (t2 != NULL);

    /* Start the threads */
    result = start_thread (t1);
    assert (result != 0);
    result = start_thread (t2);
    assert (result != 0);

    /* Wait for threads to complete */
    result = join_thread (t1);
    assert (result == 50);
    result = join_thread (t2);
    assert (result == 50);

    /* Release threads */
    delete_thread (t1);
    delete_thread (t2);
}


/* Increment counter once */
static int
increments (thread_t *tp)
{
    int value;

    /* Ignore parameter */
    (void) tp;

    /* Increment and return old count */
    enter_critical ();
    value = counter++;
    leave_critical ();
    return value;
}


/* Increment odd counter */
static int
paddle_odd (thread_t *tp)
{
    int result = 0;

    /* Ignore parameter */
    (void) tp;

    while (!result) {
        enter_critical ();
        if (counter < 50) {
            if ((counter & 2) == 1) {
                /* Odd count */
                counter++;
            } else {
                /* Even count */
                /*NOP*/;
            }
        } else {
            result = counter;
        }
        leave_critical ();
        yield ();
    }
    return result;
}


/* Increment event counter */
static int
paddle_even (thread_t *tp)
{
    int result = 0;

    /* Ignore parameter */
    (void) tp;

    while (!result) {
        enter_critical ();
        if (counter < 50) {
            if ((counter & 2) == 1) {
                /* Odd count */
                /*NOP*/;
            } else {
                /* Even count */
                counter++;
            }
        } else {
            result = counter;
        }
        leave_critical ();
        yield ();
    }
    return result;
}

