/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/thread.h"
#include "t7/fixture.h"
#include "t7/memory.h"
#include "t7/critical-section.h"


/* Internal implementation data */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/
    struct thread_info {
        int running;
        int retval;
    };

#elif !defined(_WIN32)

    /****** Linux/Unix ******/
    static void *entry (void *arg);
    struct thread_info {
        int running;
        pthread_t id;
        fixture_t fixture;
    };

#else

    /****** Microsoft Windows ******/
    struct thread_info {
        int running;
    };

#endif
typedef struct thread_info thread_info_t;

/* Threads table */
static size_t num_threads = 0;
static thread_info_t threads[T7_MAX_THREADS];


/* Returns true if multiple concurrent threads are supported */
int
has_threads (void)
{
    int ok;

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/
    ok = 0;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/
    ok = 1;

#else

    /****** Microsoft Windows ******/
    ok = 0;

#endif

    return ok;
}


/* Construct new thread */
thread_t *
new_thread (const thread_type_t *typ)
{
    thread_t *tp;

    /* Make sure that all functions are provided */
    assert (typ != NULL);
    assert (typ->allocate != NULL);
    assert (typ->free != NULL);
    assert (typ->create != NULL);
    assert (typ->destroy != NULL);
    assert (typ->run != NULL);

    /* Construct thread object */
    tp = typ->allocate ();
    if (tp != NULL) {

        /* Initialize object */
        if (typ->create (tp, typ)) {

            /* Success */
            assert (tp->type == typ);
            assert (tp->impl == NULL);

        } else {

            /* Initialization failed */
            typ->free (tp);
            tp = NULL;

        }

    } else {

        /* Cannot allocate thread */
        tp = NULL;

    }
    return tp;
}


/* Remove old thread */
void
delete_thread (thread_t *tp)
{
    if (tp) {
        const thread_type_t *type = tp->type;

        /* Destroy thread */
        assert (type->destroy != NULL);
        type->destroy (tp);

        /* Release the thread structure itself */
        assert (type->free != NULL);
        type->free (tp);

    }
}


/* Allocate memory for default thread structure */
thread_t*
allocate_thread (void)
{
    return allocate_memory (sizeof (thread_t));
}


/* Release thread structure */
void
free_thread (thread_t *tp)
{
    /* Reset fields */
#ifndef NDEBUG
    tp->type = (thread_type_t*) -1;
    tp->impl = (void*) -1;
#endif

    /* Release memory */
    free_memory (tp);
}


/* Create thread but don't start it yet */
int
create_thread (thread_t *tp, const thread_type_t *typ)
{
    /* Pre-conditions */
    assert (tp != NULL);
    assert (typ->allocate != NULL);
    assert (typ->free != NULL);
    assert (typ->create != NULL);
    assert (typ->destroy != NULL);
    assert (typ->run != NULL);

    /* Initialize fields */
    tp->type = typ;
    tp->impl = NULL;

    return 1;
}


/* Release thread */
void
destroy_thread (thread_t *tp)
{
    /* Pre-conditions */
    assert (tp != NULL);
    assert (tp->impl == NULL);

    /* Reset fields */
#ifndef NDEBUG
    tp->type = (thread_type_t*) -1;
    tp->impl = (void*) -1;
#endif
}


/* Run a thread */
int
start_thread (thread_t *tp)
{
    int ok;
    thread_info_t *ip;

    /* Cannot start the same thread object twice */
    if (tp->impl == NULL) {

        /* Lock out other threads */
        enter_critical ();

        /* Allocate implementation structure */
        if (num_threads < T7_MAX_THREADS) {

            /* Lots of room left, just take next vacant structure */
            ip = &threads[num_threads++];
            ip->running = 1;
            tp->impl = ip;

        } else {

            /* All slots used, find first free slot */
            ip = &threads[0];
            while (ip != &threads[T7_MAX_THREADS]) {
                if (ip->running == 0) {
                    /* Found an unused slot */
                    ip->running = 1;
                    tp->impl = ip;
                    break;
                }
                ip++;
            }

        }

        /* Allow other threads */
        leave_critical ();

        /* Start the native thread */
        if (ip != &threads[T7_MAX_THREADS]) {

#if defined(T7_DISABLE_THREADS)

            /****** Single Threaded ******/
            fixture_t *orig;
            fixture_t tmp;

            /* Take hold of current fixture */
            orig = get_fixture ();

            /* Initialize new temporary fixture */
            copy_fixture (&tmp, orig);
            set_fixture (&tmp);

            /* Execute the thread function */
            ip->retval = tp->type->run (tp);
            ok = 1;

            /* Restore original fixture */
            set_fixture (orig);

#elif !defined(_WIN32)

            /****** Linux/Unix ******/
            pthread_attr_t attr;

            /* Initialize fixture for the new thread */
            copy_fixture (&ip->fixture, get_fixture ());

            /* Create thread attribute */
            if (pthread_attr_init (&attr) == /*OK*/0) {

                /* Start the thread */
                if (pthread_create (&ip->id, &attr, entry, tp) == /*OK*/0) {

                    /* Success, thread now running */
                    ok = 1;

                } else {

                    /* Cannot start a thread */
                    ok = 0;

                }

                /* Release attributes */
                pthread_attr_destroy (&attr);
            }

#else

            /****** Microsoft Windows ******/

            /* FIXME: */
            terminate ("Threads not implemented yet");

#endif

        } else {

            /* Too many concurrent threads */
            /* FIXME: error() */
            ok = 0;

        }

    } else {

        /* Thread already started */
        /* FIXME: error() */
        ok = 0;

    }
    return ok;
}


/* Thread's entry point */
#if !defined(T7_DISABLE_THREADS) && !defined(_WIN32)
static void *
entry (void *arg)
{
    int result;
    thread_info_t *ip;

    /* Convert argument to thread object */
    thread_t *tp = (thread_t*) arg;

    /* Set up fixture for the current thread */
    ip = (thread_info_t*) tp->impl;
    assert (ip != NULL);
    set_fixture (&ip->fixture);

    /* Execute thread function */
    result = tp->type->run (tp);

    /* Cast integer to void pointer as required by pthreads */
    return (void*) (size_t) result;
}
#endif


/* Wait for thread to finish */
int
join_thread (thread_t *tp)
{
    int ok;
    thread_info_t *ip;

    /* Pre-conditions */
    assert (tp != NULL);

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/
    if (tp->impl) {

        /* Get return value from implementation data */
        ip = (thread_info_t*) tp->impl;
        ok = ip->retval;

        /* Mark the thread as finished */
        tp->impl = NULL;
        ip->running = 0;

    } else {

        /* Thread not run */
        ok = 0;

    }

#elif !defined(_WIN32)

    /****** Linux/Unix ******/
    if (tp->impl) {
        void *retval;

        /* Get pointer to implementation data */
        ip = (thread_info_t*) tp->impl;

        /* Wait for thread to finish */
        if (pthread_join (ip->id, &retval) == /*OK*/0) {

            /* Success, cast return value to integer */
            ok = (int) (size_t) retval;

            /*
             * Mark the thread as terminated.  This allows the implementation
             * data to be used for creating another thread.
             */
            tp->impl = NULL;
            ip->running = 0;

        } else {

            /* Deadblock or invalid thread */
            ok = 0;

        }

    } else {

        /* Thread already joined? */
        /* FIXME: error() */
        ok = 0;

    }

#else

    /****** Microsoft Windows ******/

    /* FIXME: */
    terminate ("Threads not implemented yet");

#endif
    return ok;
}


/* Release CPU to other threads */
void
yield (void)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/
    /*NOP*/;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/
    sched_yield ();

#else

    /****** Microsoft Windows ******/

    /* FIXME: */

#endif
}

