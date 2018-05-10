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


/* Operating system specific variables */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Mutex */
    static pthread_mutex_t critical_section = PTHREAD_MUTEX_INITIALIZER;

#else

    /****** Microsoft Windows ******/

    /* FIXME: */

#endif


/* For saving exit handlers */
struct exit_handler {
    exit_function *f;
    int priority;
};
typedef struct exit_handler exit_handler_t;


/* Local functions */
static void run_exit_handlers (void);
static int add_exit_handler (exit_function *f, int priority);
static int is_registered (exit_function *f);
static void enter_protected (void);
static void leave_protected (void);


/* List of registered exit handlers */
static exit_handler_t handlers[T7_MAX_EXIT_HANDLERS];

/* Number of registered exit handlers */
static size_t count = 0;

/* True if exit handler is initialized */
static int initialized = 0;


/* Register function to be called at program exit */
int
exit_handler (exit_function *f, int priority)
{
    int ok;

    /* Pre-conditions */
    assert (f != NULL);

    /* Has the module been initialized yet? */
    if (initialized) {

        /* Yes, continue with the insertion */
        ok = add_exit_handler (f, priority);

    } else {

        /* Instruct system to call our exit function */
        if (atexit (run_exit_handlers) == /*OK*/0) {

            /* System exit function set up */
            initialized = 1;

            /* Continue with the insertion */
            ok = add_exit_handler (f, priority);

        } else {

            /* Error */
            terminate ("Cannot register atexit function");
            ok = 0;

        }
    }

    return ok;
}


/* Add exit function */
static int
add_exit_handler (exit_function *f, int priority)
{
    int ok;

    /* Pre-conditions */
    assert (initialized);
    assert (f != NULL);

    /* Lock other threads */
    enter_protected ();

    /* Avoid registering duplicate handlers */
    if (!is_registered (f)  &&  count < T7_MAX_EXIT_HANDLERS) {
        size_t i;
        size_t j;

        /*
         * Find position i in the handler table such that exit handlers remain
         * ordered from highest to lowest priority after the new handler is
         * inserted to position i.
         */
        i = 0;
        while (i < count  &&  priority < handlers[i].priority) {
            i++;
        }

        /* Shift handlers down to make room for new handler */
        for (j = count; j > i; j--) {
            handlers[j] = handlers[j-1];
        }

        /* Store new handler at position i */
        handlers[i].f = f;
        handlers[i].priority = priority;

        /* Register slot as taken */
        count++;

        /* Return with non-zero exit value */
        ok = 1;

    } else if (count < T7_MAX_EXIT_HANDLERS) {

        /* Trying to add same handler twice */
        /* FIXME: error() */
        ok = 0;

    } else {

        /* Too many exit handlers */
        terminate ("Too many exit functions");
        ok = 0;

    }

    leave_protected ();
    return ok;
}


/* Returns true if exit function has already been registered */
static int
is_registered (exit_function *f)
{
    size_t i;
    int registered = 0;

    /* Check every registered function */
    for (i = 0; i < count; i++) {
        if (handlers[i].f == f) {
            registered = 1;
            break;
        }
    }
    return registered;
}


/* Invoke exit functions */
static void
run_exit_handlers (void)
{
    size_t i;

    /*
     * Loop through exit functions.
     *
     * At this point no threads should be running!  If a thread is running
     * while the exit functions are called, then the application will crash.
     */
    for (i = 0; i < count; i++) {

        /* Invoke ith exit function */
        if (handlers[i].f != NULL) {
            handlers[i].f ();
        }

    }
}


/* Enter critical section */
static void
enter_protected (void)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /*NOP*/;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Acquire lock */
    if (pthread_mutex_lock (&critical_section) != /*OK*/0) {
        terminate ("Cannot aqcuire mutex");
    }

#else

    /****** Microsoft Windows ******/

    /* FIXME: */
    terminate ("Not implemented yet");

#endif
}


/* Leave critical section */
static void
leave_protected (void)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /*NOP*/;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Release lock */
    if (pthread_mutex_unlock (&critical_section) != /*OK*/0) {
        terminate ("Cannot release mutex");
    }

#else

    /****** Microsoft Windows ******/

    /* FIXME: */
    terminate ("Not implemented yet");

#endif
}

