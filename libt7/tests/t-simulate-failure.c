/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/simulate-failure.h"
#include "t7/thread.h"

#undef NDEBUG
#include <assert.h>

/* Local test functions */
static int never (void);
static int always (void);
static int second (void);
static int fourth (void);
static int recursive (void);
static int in_thread (void);
static int thread_main (thread_t *tp);

/* For keeping up with the number of test runs */
static int counter = 0;

/* Define thread types */
static thread_type_t def1 = {
    allocate_thread,
    free_thread,
    create_thread,
    destroy_thread,
    thread_main
};
static thread_type_t *my_thread = &def1;


int
main (void)
{
    int ok;

    /* Execute tests in the main thread */
    ok = thread_main (NULL);
    assert (ok);

    /* Execute tests in a sub-thread */
    ok = repeat_test (in_thread);
    assert (ok);

    return 0;
}


/* Execute tests in current thread */
static int
thread_main (thread_t *tp)
{
    int ok;

    /* Ignore parameter */
    (void) tp;

    /* Execute test functions non-recursively */
    ok = recursive ();
    assert (ok);

    /* Execute tests functions recursively */
    ok = repeat_test (recursive);
    assert (ok);

    return 1;
}


/* Execute test in current recursion level */
static int
recursive (void)
{
    int ok;

    /* If failure is not simulated, then the test passes on the first try */
    counter = 0;
    ok = repeat_test (always);
    assert (ok);
    assert (counter == 1);

    /* Test passes on second time, if failure is simulated once */
    counter = 0;
    ok = repeat_test (second);
    assert (ok);
    assert (counter == 2);

    /* Test passes on fourth time if failure is simulated 3 times */
    counter = 0;
    ok = repeat_test (fourth);
    assert (ok);
    assert (counter == 4);

    /* Test fails if the test function never returns true */
    counter = 0;
    ok = repeat_test (never);
    assert (!ok);
    assert (counter == 1);

    return 1;
}


/* Test in a thread */
static int
in_thread (void)
{
    thread_t *t1;
    int result;

    /* Create thread */
    t1 = new_thread (my_thread);
    assert (t1 != NULL);

    /* Start the thread */
    result = start_thread (t1);
    assert (result != 0);

    /* Wait for the thread to complete */
    result = join_thread (t1);
    assert (result);

    /* Destroy thread */
    delete_thread (t1);
    return 1;
}


/* Test function which never returns true */
static int
never (void)
{
    counter++;
    return 0;
}


/* Test function which always returns true */
static int
always (void)
{
    counter++;
    return 1;
}


/* Test function which simulates failure once but succeeds on second try */
static int
second (void)
{
    int ok;
    counter++;
    if (!simulate_failure ()) {
        ok = 1;
    } else {
        ok = 0;
    }
    return ok;
}


/* Test function which succeeds at fourth iteration */
static int
fourth (void)
{
    int ok;
    counter++;
    if (!simulate_failure ()) {
        if (!simulate_failure ()) {
            if (!simulate_failure ()) {
                /* Success at fourth */
                ok = 1;
            } else {
                /* Fail at third attempt */
                ok = 0;
            }
        } else {
            /* Fail at second attempt */
            ok = 0;
        }
    } else {
        /* Fail at first attempt */
        ok = 0;
    }
    return ok;
}

