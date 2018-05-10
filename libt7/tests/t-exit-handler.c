/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/exit-handler.h"
#include "t7/terminate.h"

#undef NDEBUG
#include <assert.h>


/* Local functions */
void exit1 (void);
void exit2 (void);
void exit3 (void);
void exit4 (void);
void exit5 (void);


/* Number of exit functions called */
static int count = 0;


int
main (void)
{
    int ok;

    /*
     * Register exit functions 1 and 2 with priority of zero.  As the priority
     * is the same, the function registered first will be invoked last.
     */
    ok = exit_handler (exit1, 0);
    assert (ok);
    ok = exit_handler (exit2, 0);
    assert (ok);

    /*
     * Register function with priority of 1.  Even though this exit function
     * is registered after functions 1 and 2, it will be invoked first.
     */
    ok = exit_handler (exit3, 1);
    assert (ok);

    /* Register function with high priority */
    ok = exit_handler (exit4, 99999);
    assert (ok);

    /* Function with lesser priority will be added in the middle */
    ok = exit_handler (exit5, 100);
    assert (ok);

    /* Trying to register same exit handler twice leads to an error */
    ok = exit_handler (exit1, 0);
    assert (!ok);

    /*
     * Return with non-zero exit status to make the test fail if the exit
     * functions are not called.
     */
    return 1;
}


void
exit4 (void)
{
    assert (count == 0);
    count++;
}


void
exit5 (void)
{
    assert (count == 1);
    count++;
}


void
exit3 (void)
{
    assert (count == 2);
    count++;
}


void
exit2 (void)
{
    assert (count == 3);
    count++;
}


void
exit1 (void)
{
    assert (count == 4);
    count++;

    /* Exit immediately with exit status of zero */
    exit_application (0);
}

