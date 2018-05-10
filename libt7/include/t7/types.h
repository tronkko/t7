/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_TYPES_H
#define T7_TYPES_H

/* Include platform dependent header files */
#ifndef _WIN32

    /* Linux/Unix */
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <assert.h>

#else

    /* Microsoft Windows */
#   include <windows.h>
#   include <assert.h>

#endif

/* Get availability of custom header files */
#include "t7/config.h"

/* Include optional header files */
#if defined(HAVE_PTHREAD_H)
#   include <pthread.h>
#endif
#if defined(HAVE_SCHED_H)
#   include <sched.h>
#endif

/* Get configuration options */
#include "t7/features.h"

#endif /*T7_TYPES_H*/

