/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/fixture.h"
#include "t7/terminate.h"


/* Default fixture providers */
static allocator_t *get_fixture_allocator (fixture_t *fp);

/* Current fixture for main thread */
static fixture_t def1 = {
    get_fixture_allocator
};
fixture_t *default_fixture = &def1;


/* Operating system specific variables */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Pointer to current fixture */
    static fixture_t *active = default_fixture;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local storage */
    static void init_pthread (void);

    /* Pthread key for thread local storage */
    static pthread_key_t key;

    /* Pthread initializer */
    static pthread_once_t key_once = PTHREAD_ONCE_INIT;

#else

    /****** Microsoft Windows ******/

    /* FIXME: */

#endif


/* Get pointer to current fixture */
fixture_t*
get_fixture (void)
{
    fixture_t *fp;

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Return pointer to global fixture */
    fp = active;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local key */
    if (pthread_once (&key_once, init_pthread) == /*OK*/0) {

        /* Does this thread have its own fixture? */
        fp = (fixture_t*) pthread_getspecific (key);
        if (fp != NULL) {

            /* Yes, return pointer to thread's fixture */
            /*NOP*/;

        } else {

            /* No, return pointer to global fixture */
            fp = default_fixture;

        }

    } else {

        /* Thread once failed */
        terminate ("Cannot initialize fixtures");
        fp = NULL;

    }

#else

    /****** Microsoft Windows ******/

    terminate ("Fixtures not implemented yet");

#endif
    return fp;
}


/* Set active fixture */
void
set_fixture (fixture_t *fp)
{
    /* Preconditions */
    assert (fp != NULL);
    assert (fp->get_fixture_allocator != NULL);

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Set active fixture */
    active = fp;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local key */
    if (pthread_once (&key_once, init_pthread) == /*OK*/0) {

        /* Set pointer to fixture */
        if (pthread_setspecific (key, fp) == /*OK*/0) {

            /* Success */
            /*NOP*/;

        } else {

            /* Error */
            terminate ("Cannot set fixture");

        }

    } else {

        /* Thread once failed */
        terminate ("Cannot initialize fixtures");

    }

#else

    /****** Microsoft Windows ******/

    terminate ("Fixtures not implemented yet");

#endif
}


/* Copy fixture object */
void
copy_fixture (fixture_t *dest, const fixture_t *src)
{
    /* Pre-conditions */
    assert (dest != NULL);
    assert (src != NULL);
    assert (src->get_fixture_allocator != NULL);

    /* Copy fixture as is */
    dest->get_fixture_allocator = src->get_fixture_allocator;
}


/* Get default allocator */
static allocator_t*
get_fixture_allocator (fixture_t *fp)
{
    (void) fp;
    return get_allocator (default_allocator);
}


/* Initialize pthread */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
init_pthread (void)
{
    /* Create fixture key in all threads */
    if (pthread_key_create (&key, NULL) != /*OK*/0) {

        /*
         * Cannot create pthread key.  This is a serious error because we have
         * no reasonable way to return error from this function.  Lets just
         * terminate the current program!
         */
        terminate ("Cannot create fixture key");

    }
}
#endif


