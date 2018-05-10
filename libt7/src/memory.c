/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/memory.h"
#include "t7/allocator.h"


/* Allocate n bytes of memory */
void *
allocate_memory (size_t n)
{
    allocator_t *ap = get_default_allocator ();
    return allocator_allocate_memory (ap, n);
}


/* Release memory area */
void
free_memory (void *p)
{
    allocator_t *ap = get_default_allocator ();
    allocator_free_memory (ap, p);
}


/* Resize memory region */
void *
resize_memory (void *p, size_t n)
{
    allocator_t *ap = get_default_allocator ();
    return allocator_resize_memory (ap, p, n);
}


/* Reset memory region */
void
zero_memory (void *p, size_t n)
{
    assert (p != NULL  ||  n == 0);

#ifndef _WIN32

    /****** Linux/Unix ******/
    memset (p, 0, n);

#else

    /****** Microsoft Windows ******/
    ZeroMemory (p, n);

#endif
}


/* Fill memory region with character */
void
fill_memory (void *p, unsigned char c, size_t n)
{
    assert (p != NULL  ||  n == 0);

#ifndef _WIN32

    /****** Linux/Unix ******/
    memset (p, (int) c, n);

#else

    /****** Microsoft Windows ******/
    FillMemory (p, n, c);

#endif
}


/* Copy memory regions */
void
copy_memory (void *p, const void *q, size_t n)
{
    /* Make sure that memory areas do not overlap */
    assert ((const char*) q + n <= (const char*) p  
            ||  (const char*) p + n <= (const char*) q
            ||  p == q);

#ifndef _WIN32

    /****** Linux/Unix ******/
    memcpy (p, q, n);

#else

    /****** Microsoft Windows ******/
    CopyMemory (p, n, n);

#endif
}


/* Move bytes within region */
void
move_memory (void *p, const void *q, size_t n)
{
    assert (p != NULL  ||  n == 0);

#ifndef _WIN32

    /****** Linux/Unix ******/
    memmove (p, q, n);

#else

    /****** Microsoft Windows ******/
    MoveMemory (p, q, n);

#endif
}


/* Swap contents of two memory regions */
void
swap_memory (void *p, void *q, size_t n)
{
    size_t i = 0;
    char *cp = (char*) p;
    char *qp = (char*) q;
    char tmp;

    /* Check against null write */
    assert ((cp != NULL  &&  qp != NULL)  ||  n == 0);

    /* Make sure that memory areas do not overlap */
    assert (qp + n <= cp  ||  cp + n <= qp  ||  cp == qp);

    /*
     * Swap memory areas byte by byte.  This is inefficient but will
     * have to do for now.
     */
    for (i = 0; i < n; i++) {
        tmp = cp[i];
        cp[i] = qp[i];
        qp[i] = tmp;
    }
}


/*
 * Allocate memory directly from system.
 *
 * Be ware that this function is called before fixtures are initialized.
 * Thus, the function cannot call other functions who might depend on
 * fixtures!
 */
void *
system_allocate_memory (size_t n)
{
    void *p;

#if !defined(_WIN32)

    /****** Linux/Unix ******/
    p = malloc (n);

#else

    /****** Microsoft Windows ******/
    p = HeapAlloc (GetProcessHeap (), 0, n);

#endif

    return p;
}


/* Release memory directly to system, not through fixtures */
void
system_free_memory (void *p)
{
#ifndef _WIN32

    /****** Linux/Unix ******/
    free (p);

#else

    /****** Microsoft Windows ******/
    HeapFree (GetProcessHeap (), 0, p);

#endif
}


/* Resize memory region directly, not through fixtures */
void *
system_resize_memory (void *p, size_t n)
{
    void *q;

#ifndef _WIN32

    /****** Linux/Unix ******/
    q = realloc (p, n);

#else

    /****** Microsoft Windows ******/
    q = HeapReAlloc (GetProcessHeap (), 0, p, n);

#endif

    return q;
}


