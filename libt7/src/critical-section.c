/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/critical-section.h"
#include "t7/terminate.h"
#include "t7/exit-handler.h"


/* Operating system specific variables */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Pthread initializer */
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    /* Mutex */
    static pthread_mutex_t critical_section;

    /* Mutex attributes */
    static pthread_mutexattr_t attr;

    /* Initialize threads */
    static void init_pthread (void);

    /* Uninitialize threads */
    static void done_pthread (void);

#else

    /****** Microsoft Windows ******/

    /* FIXME: */

#endif


/* Enter critical section of code */
void
enter_critical (void)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single threaded ******/
    /*NOP*/;

#elif !defined(WIN32)

    /****** Linux/Unix ******/

    /* Initialize mutex on first time */
    if (pthread_once (&once, init_pthread) == /*OK*/0) {

        /* Acquire mutex */
        if (pthread_mutex_lock (&critical_section) == /*OK*/0) {

            /* Success */
            /*NOP*/;

        } else {
            terminate ("Cannot aqcuire mutex");
        }

    } else {
        terminate ("Cannot initialize mutex");
    }

#else

    /****** Microsoft Windows ******/
    /* FIXME: */
    terminate ("Not implemented yet");

#endif
}


/* Level critical section */
void
leave_critical (void)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single threaded ******/

    /*NOP*/;

#elif !defined(WIN32)

    /****** Linux/Unix ******/

    /* Make sure that module has been initialized properly */
    assert (once != PTHREAD_ONCE_INIT);

    /* Release mutex */
    if (pthread_mutex_unlock (&critical_section) == /*OK*/0) {

        /* Success */
        /*NOP*/;

    } else {
        terminate ("Cannot release mutex");
    }

#else

    /****** Microsoft Windows ******/
    /* FIXME: */
    terminate ("Not implemented yet");

#endif
}


/* Initialize pthread mutex */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
init_pthread (void)
{
    /* Initialize attributes object */
    if (pthread_mutexattr_init (&attr) != /*OK*/0) {
        terminate ("Cannot allocate mutex attribute");
    }

    /*
     * Set the mutex to recursive.  That is, an attempt to lock the same
     * mutex twice by the same thread will increment the lock count by one
     * instead of deadlocking.
     */
    if (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
        terminate ("Cannot set mutex type");
    }

    /* Initialize mutex with attributes */
    if (pthread_mutex_init (&critical_section, &attr) != /*OK*/0) {
        terminate ("Cannot initialize mutex");
    }

    /* Install exit handler to clean up the module afterwards */
    if (!exit_handler (done_pthread, 10)) {
        terminate ("Cannot install exit handler");
    }
}
#endif


/* Un-initialize pthread mutex */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
done_pthread (void)
{
    /* Release mutex */
    pthread_mutex_destroy (&critical_section);

    /* Release attributes */
    pthread_mutexattr_destroy (&attr);

    /* Mark the module not initialized */
    once = PTHREAD_ONCE_INIT;
}
#endif

