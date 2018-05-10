/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/critical-section.h"

#undef NDEBUG
#include <assert.h>


/* Static variables */
static int counter = 0;


/* Prototypes */
static void recursive_test (void);


int
main (void)
{
    /* Update variable in a critical section */
    assert (counter == 0);
    recursive_test ();
    assert (counter == 1);

    /* Critical section may be entered recursively */
    enter_critical ();
    recursive_test ();
    leave_critical ();
    assert (counter == 2);

    return 0;
}


/* Update variable in a critical section */
static void
recursive_test (void)
{
    enter_critical ();
    counter++;
    leave_critical ();
}

