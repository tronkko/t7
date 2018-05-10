/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/tls.h"
#include "t7/memory.h"
#include "t7/exit-handler.h"
#include "t7/terminate.h"


/* Declarations */
struct storage {
    tls_variable_t *first;
};
typedef struct storage storage_t;


/* Prototypes */
static storage_t *get_storage (void);
static storage_t *new_storage (void);
static void delete_storage (storage_t *sp);
static int create_storage (storage_t *sp);
static void destroy_storage (storage_t *sp);
static tls_variable_t *new_tls (const tls_type_t *tp);
static void delete_tls (tls_variable_t *vp);


/* Operating system specific variables */
#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Storage for single-threaded operation */
    static storage_t *global_storage = NULL;

    /* Un-initialize global storage at exit */
    static void single_thread_exit (void);

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thread local storage */
    static void init_pthread (void);

    /* Uninitialize thread local storage */
    static void done_pthread (void *p);

    /* Clean up main thread */
    static void multi_thread_exit (void);

    /* Pthread key for thread local storage */
    static pthread_key_t key;

    /* Pthread initializer */
    static pthread_once_t key_once = PTHREAD_ONCE_INIT;

#else

    /****** Microsoft Windows ******/

    /* FIXME: */

#endif


/* Get value of thread-local variable */
void *
get_tls (const tls_type_t *tp)
{
    storage_t *sp;
    tls_variable_t *vp;
    void *data;
    int found;

    /* Get pointer to current storage object */
    sp = get_storage ();
    if (sp != NULL) {
        data = NULL;
        found = 0;

        /* Find variable with type tp from list */
        vp = sp->first;
        while (vp != NULL) {

            /* Is this the variable we are looking for? */
            if (vp->type != tp) {

                /* No, check next variable */
                vp = vp->next;

            } else {

                /* Yes, value found */
                assert (vp->type->get != NULL);
                data = vp->type->get (vp);
                found = 1;
                break;

            }

        }

        /* Create new variable if type was not registered before */
        if (!found) {

            /* Allocate room for variable */
            vp = new_tls (tp);
            if (vp) {

                /* Add variable to the beginning of list */
                vp->next = sp->first;
                sp->first = vp;

                /* Return variable data */
                assert (vp->type->get != NULL);
                data = vp->type->get (vp);

            } else {

                /* Cannot allocate new variable */
                data = NULL;

            }

        }

    } else {

        /* Cannot initialize storage */
        data = NULL;

    }
    return data;
}


/* Get pointer to thead-local storage */
static storage_t*
get_storage (void)
{
    storage_t *sp;

#if defined(T7_DISABLE_THREADS)

    /****** Single Threaded ******/

    /* Are variables initialized already? */
    if (global_storage) {

        /* Yes, just get pointer to global tls object */
        sp = global_storage;

    } else {

        /* No, register cleanup function */
        if (exit_handler (single_thread_exit, 40)) {

            /* Initialize global storage */
            global_storage = new_storage ();
            if (global_storage) {

                /* Return pointer to global storage */
                sp = global_storage;

            } else {

                /* Initialization failure */
                sp = NULL;

            }

        } else {

            /* Cannot register exit handler */
            sp = NULL;

        }

    }

#elif !defined(_WIN32)

    /****** Linux/Unix ******/

    /* Initialize thead local key */
    if (pthread_once (&key_once, init_pthread) == /*OK*/0) {

        /* See if the storage has been created for this thread? */
        sp = (storage_t*) pthread_getspecific (key);
        if (sp != NULL) {

            /* Yes, storage created */
            /*NOP*/;

        } else {

            /* Storage not yet created */
            sp = new_storage ();
            if (sp) {

                /* Save storage to thread local storage */
                if (pthread_setspecific (key, sp) == /*OK*/0) {

                    /* Success */
                    /*NOP*/;

                } else {

                    /* Cannot save storage to TLS key */
                    /* FIXME: error() */
                    delete_storage (sp);
                    sp = NULL;

                }

            } else {

                /* Cannot allocate storage */
                sp = NULL;

            }

        }

    } else {

        /* Cannot initialize TLS key */
        /* FIXME: error() */
        sp = NULL;

    }

#else

    /****** Microsoft Windows ******/

    /* FIXME: */
    terminate ("TLS not implemented yet");

#endif
    return sp;
}


/* Allocate storage object */
static storage_t *
new_storage (void)
{
    storage_t *sp;

    /* Allocate memory for storage */
    sp = system_allocate_memory (sizeof (storage_t));
    if (sp) {

        /* Initialize storage */
        if (create_storage (sp)) {

            /* Success */
            /*NOP*/;

        } else {

            /* Initialization failure */
            system_free_memory (sp);
            sp = NULL;

        }

    } else {

        /* Out of memory */
        sp = NULL;

    }
    return sp;
}


/* Release storage object */
static void
delete_storage (storage_t *sp)
{
    if (sp != NULL) {

        /* Un-initialize storage */
        destroy_storage (sp);

        /* Release storage structure itself */
        system_free_memory (sp);

    }
}


/* Initialize storage */
static int
create_storage (storage_t *sp)
{
    /* Reset storage object */
    sp->first = NULL;

    /* Return true to indicate success */
    return 1;
}


/* Un-initialize storage */
static void
destroy_storage (storage_t *sp)
{
    tls_variable_t *vp;

    /* Loop through variables and release them */
    vp = sp->first;
    while (vp != NULL) {
        tls_variable_t *next;

        /* Get pointer to next tls variable */
        next = vp->next;

        /* Remove variable vp from list */
        sp->first = next;

        /* Release variable vp */
        delete_tls (vp);

        /* Continue with next variable */
        vp = next;

    }
    assert (sp->first == NULL);
}


/* Allocate tls variable */
static tls_variable_t *
new_tls (const tls_type_t *tp)
{
    tls_variable_t *vp;

    /* Pre-conditions */
    assert (tp->allocate != NULL);
    assert (tp->free != NULL);
    assert (tp->get != NULL);

    /* Allocate room for TLS variable */
    vp = tp->allocate ();
    if (vp) {

        /* Initialize custom fields */
        if (tp->create (vp, tp)) {

            /* Success */
            /*NOP*/;

        } else {

            /* Initialization failure */
            tp->free (vp);
            vp = NULL;

        }

    } else {

        /* Run out of memory? */
        vp = NULL;

    }
    return vp;
}


/* Initialize generic TLS variable */
int
create_tls (tls_variable_t *vp, const tls_type_t *tp)
{
    /* Initialize base object */
    vp->next = NULL;
    vp->type = tp;

    /* Success */
    return 1;
}


/* Un-initialize generic TLS variable */
void
destroy_tls (tls_variable_t *vp)
{
    /* Pre-conditions */
    assert (vp != NULL);

    /* Reset fields */
#ifndef NDEBUG
    vp->next = (tls_variable_t*) ~0;
    vp->type = (const tls_type_t*) ~0;
#endif
}


/* Release tls variable */
static void
delete_tls (tls_variable_t *vp)
{
    if (vp) {
        const tls_type_t *type = vp->type;

        /* Un-initialize custom fields */
        assert (type->destroy != NULL);
        type->destroy (vp);

        /* Release variable structure */
        assert (type->free != NULL);
        type->free (vp);

    }
}


/* Initialize thread local storage */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
init_pthread (void)
{
    /* Create key in all threads */
    if (pthread_key_create (&key, done_pthread) != /*OK*/0) {

        /*
         * Cannot create pthread key.  This is a serious error because we have
         * no reasonable way to return error from this function.  Lets just
         * terminate the current program!
         */
        terminate ("Cannot create TLS key");

    }

    /* Register cleanup function for main thread */
    if (!exit_handler (multi_thread_exit, 40)) {
        terminate ("Cannot register exit handler");
    }
}
#endif


/* Un-initialize thread local storage when thread exits */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
done_pthread (void *p)
{
    /* Release storage structure */
    storage_t *sp = (storage_t*) p;
    if (sp != NULL) {
        delete_storage (sp);
    }
}
#endif


/*
 * Clean up after main thread.
 *
 * Please recall that we register the destructor function done_pthread when
 * creating the thread-local key.  However, due to the design, the destructor
 * function is not called when exiting the main thread!  Instead, we will have
 * to clean the main thread in an exit handler.  This allows valgrind and
 * other memory debuggers to report memory allocation errors properly.
 */
#if !defined(T7_DISABLE_THREADS)  &&  !defined(_WIN32)
static void
multi_thread_exit (void)
{
    storage_t *sp;

    /* Get current storage */
    sp = (storage_t*) pthread_getspecific (key);
    if (sp != NULL) {

        /* Reset thread-local variable (ignore possible error) */
        pthread_setspecific (key, NULL);

        /* Release storage object */
        delete_storage (sp);

    }
}
#endif


/* Un-initialize global storage at exit */
#if defined(T7_DISABLE_THREADS)
static void
single_thread_exit (void)
{
    if (global_storage) {
        delete_storage (global_storage);
        global_storage = NULL;
    }
}
#endif

