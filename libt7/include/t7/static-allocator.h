/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_STATIC_ALLOCATOR_H
#define T7_STATIC_ALLOCATOR_H
#include "t7/allocator.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Structure of internal memory node */
struct static_node {
    size_t size;
};
typedef struct static_node static_node_t;


/****s* libt7/static_allocator_data
 * NAME
 * static_allocator_data - structure of static allocator
 *
 * FUNCTION
 * Structure of static memory allocator.  See static_allocator_t for more
 * information.
 *
 * SOURCE
 */
struct static_allocator_data {

    /* Base allocator, must be first member of the structure */
    allocator_t base;

    /* Pointer to next memory node to check */
    static_node_t *start;

    /* Pointer to memory buffer */
    char *buffer;

    /* Total size of memory buffer in bytes */
    size_t size;

};
/****/


/****t* libt7/static_allocator_t
 * NAME
 * static_allocator_t - static memory allocator
 *
 * FUNCTION
 * A memory allocator which uses a pre-allocated one-megabyte memory buffer.
 * This memory buffer can be used as a parachute when the operating system
 * runs out of memory.
 *
 * The static allocator can be further derived to provide a statically
 * allocated buffer of any given size which avoids using operating system
 * memory allocation functions altogether.  See below for an example.
 *
 * EXAMPLE
 * #include "t7/static-allocator.h"
 * #include "t7/memory.h"
 * #include "t7/terminate.h"
 *
 * // Prototypes
 * static allocator_t *my_allocate (void);
 * static void my_free (allocator_t *ap);
 * static int my_create (allocator_t *ap, const allocator_type_t *tp);
 * static void my_destroy (allocator_t *ap);
 *
 * // Define custom allocator type
 * static allocator_type_t def = {
 *     my_allocate,
 *     my_free,
 *     my_create,
 *     my_destroy,
 *     static_grab_memory,
 *     static_release_memory,
 *     static_resize_memory,
 * };
 * static const allocator_type_t *my_allocator = &def;
 *
 * // Single allocator instance.  Normally, we would define a new structure
 * // to hold the custom fields but since we don't have any custom fields
 * // here, we can do with the basic static allocator.
 * static static_allocator_t singleton;
 *
 * // Static memory buffer from where all memory is allocated
 * static char buffer[1024];
 *
 * // Allocate room for the allocator structure
 * static allocator_t *my_allocate (void) {
 *     return (allocator_t*) &singleton;
 * }
 *
 * // Release allocator structure
 * static void my_free (allocator_t *ap) {
 *    (void) ap;
 * }
 *
 * // Initialize allocator with buffer
 * static int my_create (allocator_t *ap, const allocator_type_t *tp) {
 *     return create_static_allocator (ap, tp, buffer, sizeof (buffer));
 * }
 *
 * // Uninitialize allocator
 * static void my_destroy (allocator_t *ap) {
 *     destroy_allocator (ap);
 * }
 *
 * // Example program
 * int main (void) {
 *     // Retrieve pointer to my_allocator
 *     allocator_t *ap = get_allocator (my_allocator);
 *     if (ap) {
 *
 *         // Allocate 1000 bytes from custom allocator
 *         char *p = allocator_allocate_memory (ap, 1000);
 *         if (p) {
 *
 *             // Success
 *             ...
 *
 *             // Release memory
 *             allocator_free_memory (ap, p);
 *
 *         } else {
 *             terminate ("Out of memory");
 *         }
 *
 *     } else {
 *         terminate ("Invalid allocator");
 *     }
 *     return 0;
 * }
 *
 * SOURCE
 */
typedef struct static_allocator_data static_allocator_t;
/****/


/****v* libt7/static_allocator
 * NAME
 * static_allocator - static memory allocator
 *
 * FUNCTION
 * A pointer to static allocator type which can be used to retrieve a pointer
 * to the allocator_t structure.
 *
 * EXAMPLE
 * // Create or return pointer to existing static memory allocator
 * allocator_t *ap = get_allocator (static_allocator);
 * if (ap) {
 *     // Allocate memory from static allocator
 *     char *p = allocator_allocate_memory (ap, 100);
 *     if (p) {
 *         // Success
 *     } else {
 *         terminate ("Out of memory");
 *     }
 * } else {
 *     terminate ("Error instantiating allocator");
 * }
 *
 * SOURCE
 */
extern const allocator_type_t *static_allocator;
/****/


/****F* libt7/create_static_allocator
 * NAME
 * create_static_allocator - initialize static allocator with buffer
 *
 * FUNCTION
 * Initialize static_allocator_t AP with the type TP and memory buffer P which
 * contains N bytes.  The function returns true if the initialization is
 * successful.  Otherwise, the function returns false.
 *
 * You will need this function to initialize custom allocators which utilize
 * the static_allocator.  See static_allocator_t for an example.
 *
 * SYNOPSIS
 */
int create_static_allocator(
    allocator_t *ap, const allocator_type_t *tp, char *buffer, size_t size);
/****/


/****F* libt7/allocate_static_allocator
 * NAME
 * allocate_static_allocator - allocate static_allocator_t
 *
 * FUNCTION
 * Allocate memory for static_allocator_t structure.  The function returns a
 * pointer to the newly allocated memory area or NULL if the allocation fails.
 *
 * You may need this function when building custom memory allocators which
 * build upon basic static_allocator.  However, if you custom allocator needs
 * any custom fields, then you should allocate the memory using some other
 * function.
 *
 * SYNOPSIS
 */
allocator_t *allocate_static_allocator (void);
/****/


/****F* libt7/free_static_allocator
 * NAME
 * free_static_allocator - release static_allocator_t
 *
 * FUNCTION
 * Release memory for static_allocator_t structure.
 *
 * You may need this function when building custom memory allocators which
 * build upon basic static_allocator.  However, if you custom allocator needs
 * any custom fields, then you should allocate the memory using some other
 * function.
 *
 * SYNOPSIS
 */
void free_static_allocator (allocator_t *ap);
/****/


/****F* libt7/static_grab_memory
 * NAME
 * static_grab_memory - allocate memory from static_allocator_t
 *
 * FUNCTION
 * Allocate N bytes of memory from the memory allocator AP.  This function
 * implements the grab_memory_function for a generic static memory allocator.
 * You can use the function in your custom allocator_type_t structure.  See
 * static_allocator_t for an example.
 *
 * SYNOPSIS
 */
void *static_grab_memory (allocator_t *ap, size_t n);
/****/


/****F* libt7/static_release_memory
 * NAME
 * static_release_memory - release memory to static_allocator_t
 *
 * FUNCTION
 * Release memory area pointed by P back to allocator AP.  This function
 * implements the release_memory_function for a generic static memory
 * allocator.  You can use the function in your custom allocator_type_t
 * structure.  See static_allocator_t for an example.
 *
 * SYNOPSIS
 */
void static_release_memory (allocator_t *ap, void *p);
/****/


/****F* libt7/static_resize_memory
 * NAME
 * static_resize_memory - resize memory area
 *
 * FUNCTION
 * Resize memory area pointed by P to contain at least N bytes.  This function
 * implements resize_memory_function for a generic static memory allocator.
 * You can use the function in your custom allocator_type_t structure.  See
 * static_allocator_t for an example.
 *
 * SYNOPSIS
 */
void *static_resize_memory (allocator_t *ap, void *p, size_t n);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_STATIC_ALLOCATOR_H*/

