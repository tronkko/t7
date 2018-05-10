/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_MEMORY_H
#define T7_MEMORY_H
#ifdef __cplusplus
extern "C" {
#endif


/****f* libt7/allocate_memory
 * NAME
 * allocate_memory - allocate chunk of memory
 *
 * FUNCTION
 * Allocate N bytes of memory from the default memory allocator and return
 * pointer to the newly allocated memory area.  If the allocation fails or N
 * is zero, then the function returns NULL.
 *
 * Be ware that the newly allocated memory area is left uninitialized.  Use
 * the function zero_memory to initialize it, if necessary.
 *
 * EXAMPLE
 * // Allocate 100 bytes of memory
 * void *p = allocate_memory (100);
 * if (p) {
 *
 *     // Initialize memory to zero
 *     zero_memory (p, 100);
 *
 *     // Release memory
 *     free_memory (p);
 *
 * } else {
 *     // Allocation failure
 *     terminate ("Out of memory");
 * }
 *
 * SYNOPSIS
 */
void *allocate_memory (size_t n);
/****/


/****f* libt7/free_memory
 * NAME
 * free_memory - release memory
 *
 * FUNCTION
 * Release memory area P which has been allocated with allocate_memory() or
 * resize_memory().
 *
 * SYNOPSIS
 */
void free_memory (void *p);
/****/


/****f* libt7/resize_memory
 * NAME
 * resize_memory - resize memory area
 *
 * FUNCTION
 * Resize the memory area pointed by P to contain at least N bytes.  If the
 * memory area cannot be resized, then the function allocates a new memory
 * area, copies the old memory area into it, releases memory area P and
 * returns a pointer to the newly allocated memory area.  If the memory area
 * cannot be resized, then the function returns NULL without affecting the
 * memory area P.
 *
 * Be ware that the contents will be unchanged up to the old size but any
 * new memory needs to be initialized prior to use.
 *
 * EXAMPLE
 * // Enlarge the memory area to 1000 bytes
 * q = resize_memory (p, 1000);
 * if (q) {
 *     // Size changed
 *     p = q;
 * } else {
 *     // Allocation failure
 *     terminate ("Out of memory");
 * }
 *
 * SYNOPSIS
 */
void *resize_memory (void *p, size_t n);
/****/


/****f* libt7/zero_memory
 * NAME
 * zero_memory - zero-fill memory area
 *
 * FUNCTION
 * Reset the first N bytes of the memory area pointed by P.
 *
 * EXAMPLE
 * // Define local variable st
 * struct my_struct st;
 *
 * // Zero-fill variable st
 * zero_memory (&st, sizeof (st));
 *
 * SYNOPSIS
 */
void zero_memory (void *p, size_t n);
/****/


/****f* libt7/fill_memory
 * NAME
 * fill_memory - fill memory area
 *
 * FUNCTION
 * Fill the first N bytes of the memory area pointed by P with the
 * constant 8-bit value C.
 *
 * SYNOPSIS
 */
void fill_memory (void *p, unsigned char c, size_t n);
/****/


/****f* libt7/copy_memory
 * NAME
 * copy_memory - copy memory area
 *
 * FUNCTION
 * Copy the first N bytes from the memory area pointed Q to the memory area
 * pointed by P.  Be ware that memory areas P and Q may not overlap.
 *
 * SYNOPSIS
 */
void copy_memory (void *p, const void *q, size_t n);
/****/


/****f* libt7/move_memory
 * NAME
 * move_memory - move memory area
 *
 * FUNCTION
 * Copy the first N bytes from the memory area pointed Q to the memory area
 * pointed by P.  This is similar to copy_memory() but memory areas P and Q
 * may overlap.
 *
 * SYNOPSIS
 */
void move_memory (void *p, const void *q, size_t n);
/****/


/****f* libt7/swap_memory
 * NAME
 * swap_memory - swap memory areas
 *
 * FUNCTION
 * Exchange N bytes between memory areas pointed P and Q.  Be ware that
 * memory areas may not overlap!
 *
 * EXAMPLE
 * // Exchange variables i and j
 * swap_memory (&i, &j, sizeof (i));
 *
 * SYNOPSIS
 */
void swap_memory (void *p, void *q, size_t n);
/****/


/* System memory allocation functions */
void *system_allocate_memory (size_t n);
void system_free_memory (void *p);
void *system_resize_memory (void *p, size_t n);


#ifdef __cplusplus
}
#endif
#endif /*T7_MEMORY_H*/
