/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/terminate.h"


/*
 * Terminate application with error message.
 *
 * It would be tempting let the terminate function to be overwritten with
 * fixtures.  However, terminate is used to handle initialization errors
 * eminating from fixtures and attempting to use fixtures here might lead to
 * infinite loops!
 */
void
terminate (const char *pc)
{
#ifndef WIN32

    /****** Linux/Unix ******/

    /* Output error message */
    printf ("%s\n", pc);

    /* Terminate application */
    abort ();

#else

    /****** Microsoft Windows ******/
    /* FIXME: */

#endif
    /*NOTREACHED*/
}


/* Terminate program with exit status */
void
exit_application (int status)
{
#ifndef WIN32

    /****** Linux/Unix ******/
    exit (status);

#else

    /****** Microsoft Windows ******/
    /* FIXME: */

#endif
    /*NOTREACHED*/
}

