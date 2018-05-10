/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/simulate-failure.h"
#include "t7/terminate.h"


/* Maximum depth of recursion, should be divisable by 8 */
#define MAX_RECURSION 1024

/* Forward-decl */
struct test_frame;
typedef struct test_frame test_frame_t;

/* Local functions */
static test_frame_t *get_frame (void);
static void set_frame (test_frame_t *fp);
static int _getbit (test_frame_t *fp, unsigned int i);
static void _setbit (test_frame_t *fp, unsigned int i, unsigned int value);


/* Parameters for the active failure simulation */
struct test_frame {

    /* Whether an error was triggered */
    int triggered;

    /* Number of times simulate_failure() has been called in this iteration */
    unsigned int count;

    /* Bit vector of simulated failures */
    unsigned char simulate[MAX_RECURSION / 8 + 1];

};


/* Operating system specific variables */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Pointer to current test frame */
    static test_frame_t *active = NULL;

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


/* Returns true when failure should be simulated */
int simulate_failure (void) {
    int ok;
    test_frame_t *fp;

    /* Get pointer to active test frame */
    fp = get_frame ();
    if (fp) {

        /* Simulation is active */
        if (fp->count < MAX_RECURSION) {

            /* Test whether to simulate failure at this time */
            if (_getbit (fp, fp->count++)) {

                /* Simulate failure */
                fp->triggered = 1;
                ok = 1;

            } else {

                /* Do not simulate failure at this time */
                ok = 0;

            }

        } else {
            terminate ("Too many allocation requests at simulate_failure");
        }

    } else {

        /* Testing is not under way => do not simulate failure */
        ok = 0;

    }
    return ok;
}


/* Branch and execute test */
int
repeat_test (test_function *f)
{
    test_frame_t *old;
    test_frame_t frame;
    unsigned int i;
    int ok;

    /*
     * Initialize test frame by setting every bit in the simulation vector to
     * 1.  This will cause simulate_failure() to return true every time thus
     * simulating total failure where nothing works.
     */
    frame.triggered = 0;
    frame.count = 0;
    for (i = 0; i < sizeof (frame.simulate); i++) {
        frame.simulate[i] = 0xff;
    }

    /* Publish new test frame */
    old = get_frame ();
    set_frame (&frame);

    /* Repeat the test for unspecified number of times */
    do {

        /* Initialize frame for new iteration */
        frame.triggered = 0;
        frame.count = 0;

        /* Execute test function with current simulation vector */
        ok = f ();

        /* Did the test function trigger a simulated failure? */
        if (frame.triggered) {

            /*
             * Yes, failure was triggered.  Find the last resource allocation
             * where failure was simulated.
             *
             * Since failure was simulated, we can be sure that there is at
             * least one bit set in the simulation vector.  Thus, we don't
             * need to check for index bounds, i.e. variable i will be zero or
             * greater at all times.
             */
            i = frame.count - 1;
            while (_getbit (&frame, i) == 0) {
                i--;
            }

            /*
             * Change the simulation vector such that the previously failed
             * allocation request can succeed in the next iteration.  By
             * removing one bit from the array every time we also make sure
             * that this function exits eventually.
             */
            assert (_getbit (&frame, i) == 1);
            _setbit (&frame, i++, 0);

            /*
             * If the program continues resource allocation, then all the
             * further allocations must fail by default.
             */
            while (i < MAX_RECURSION) {
                _setbit (&frame, i++, 1);
            }

        } else {

            /*
             * The test function did not trigger a failure so the testing is
             * over: the final result of the test function gives the final
             * result of the whole test.
             */
            break;

        }

    } while (1);

    /* Restore old test frame */
    set_frame (old);

    return ok;
}


/* Get ith bit */
static int
_getbit (test_frame_t *fp, unsigned int i)
{
    unsigned int offset;
    unsigned char shift;

    /* Preconditions */
    assert (fp != NULL);
    assert (i < MAX_RECURSION);

    /* Compute the number of byte where ith bit resides */
    offset = i / 8;

    /*
     * Compute the number of bits we need to shift in order to transfer ith
     * bit to the position zero.
     */
    shift = i & 7;

    /* Get value of ith bit */
    return (fp->simulate[offset] >> shift) & 1;
}


/* Set ith bit in test frame */
static void
_setbit (test_frame_t *fp, unsigned int i, unsigned int value)
{
    unsigned int offset;
    unsigned char mask;
    unsigned char shift;

    /* Preconditions */
    assert (fp != NULL);
    assert (value == 0  || value == 1);
    assert (i < MAX_RECURSION);

    /* Compute the number of byte where ith bit resides */
    offset = i / 8;

    /*
     * Compute the number of bits we need to shift the value in order to set
     * the ith bit.
     */
    shift = i & 7;

    /*
     * Compute the bit mask that zeroes the ith bit.  For example, if we are
     * setting the bit number 3, then the mask will be:
     *
     *     bit  | 7 6 5 4 3 2 1 0
     *     -----+----------------
     *     mask | 1 1 1 1 0 1 1 1
     *
     * or 0xf7 in hexadecimal.
     */
    mask = ~(1 << shift);

    /* Set or clear value of ith bit */
    fp->simulate[offset] = (fp->simulate[offset] & mask) | (value << shift);
}


/* Get pointer to active test frame */
static test_frame_t*
get_frame (void)
{
    test_frame_t *fp;

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Return pointer to global variable */
    fp = active;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local key */
    if (pthread_once (&key_once, init_pthread) == /*OK*/0) {

        /* Get pointer to test frame */
        fp = (test_frame_t*) pthread_getspecific (key);

    } else {

        /* Thread once failed */
        terminate ("Cannot initialize failure simulation");
        fp = NULL;

    }

#else

    /****** Microsoft Windows ******/

    terminate ("Failure simulation not implemented yet");

#endif
    return fp;
}


/* Set pointer to active test frame */
static void
set_frame (test_frame_t *fp)
{
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Set pointer to global variable */
    active = fp;

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local key */
    if (pthread_once (&key_once, init_pthread) == /*OK*/0) {

        /* Set pointer to test frame */
        if (pthread_setspecific (key, fp) == /*OK*/0) {

            /* Success */
            /*NOP*/;

        } else {

            /* Error */
            terminate ("Cannot set test frame");

        }

    } else {

        /* Thread once failed */
        terminate ("Cannot initialize failure simulation");
        fp = NULL;

    }

#else

    /****** Microsoft Windows ******/

    terminate ("Failure simulation not implemented yet");

#endif
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
        terminate ("Cannot initialize failure simulation");

    }
}
#endif

