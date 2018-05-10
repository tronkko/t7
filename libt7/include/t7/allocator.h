/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_ALLOCATOR_H
#define T7_ALLOCATOR_H
#ifdef __cplusplus
extern "C" {
#endif

/* Forward-decl */
struct allocator_data;
struct allocator_type;


/****t* libt7/allocator_t
 * NAME
 * allocator_t - memory allocator
 *
 * FUNCTION
 * Structure of a memory allocator.  See allocator_data for a list of
 * fields.
 *
 * Use functions get_allocator or new_allocator to construct memory
 * allocators.  The T7 library provides ready-made memory allocators
 * default_allocator and static_allocator for you.  You can also define your
 * own custom memory allocators.  See allocator_type_t for an example.
 *
 * EXAMPLE
 * // Retrieve pointer to static memory allocator
 * allocator_t *ap = get_allocator (static_allocator);
 * if (ap) {
 *     // Success
 * } else {
 *     // Failure
 * }
 *
 * SOURCE
 */
typedef struct allocator_data allocator_t;
/****/


/****t* libt7/allocator_type_t
 * NAME
 * allocator_type_t - allocator type
 *
 * FUNCTION
 * A structure with pointers to various functions which define the
 * functionality of the memory allocator.  See allocator_type for a
 * list of fields.
 *
 * The T7 library provides ready-made memory allocator types default_allocator
 * and static_allocator for you.  See below for an example on how to define
 * your own custom memory allocator types.
 *
 * EXAMPLE
 * // Build a custom memory allocator
 * #include "t7/allocator.h"
 * #include "t7/terminate.h"
 *
 * // Prototypes of custom allocator functions (you must implement these)
 * static allocator_t *allocate_my_allocator (void);
 * static void free_my_allocator (allocator_t *ap);
 * static int create_my_allocator (allocator_t *ap, const allocator_type_t *tp);
 * static void destroy_my_allocator (allocator_t *ap);
 * static void *my_grab_memory (allocator_t *ap, size_t n);
 * static void my_release_memory (allocator_t *ap, void *p);
 * static void *my_resize_memory (allocator_t *ap, void *p, size_t n);
 *
 * // Define custom allocator type
 * static allocator_type_t def = {
 *     allocate_my_allocator,
 *     free_my_allocator,
 *     create_my_allocator,
 *     destroy_my_allocator,
 *     my_grab_memory,
 *     my_release_memory,
 *     my_resize_memory,
 * };
 *
 * // Define shorthand for type
 * static const allocator_type_t *my_allocator = &def;
 *
 * // Layout of derived memory allocator
 * struct my_allocator_struct {
 *     // Derive from generic allocator (must be the first field of struct)
 *     allocator_t base;
 *
 *     // Add fields for the custom allocator
 *     char *buffer;
 *     size_t size;
 * };
 *
 * // Define shorthand for custom type
 * typedef struct my_allocator_struct my_allocator_t;
 *
 * // Allocate memory from the custom allocator
 * int main (void) {
 *     allocator_t *ap = get_allocator (my_allocator);
 *     if (ap) {
 *         // Allocate memory
 *         void *p = allocator_allocate_memory (ap, 1000);
 *         if (p) {
 *
 *             // Success
 *             ...
 *
 *             // Release memory back to allocator
 *             allocator_free_memory (ap, p);
 *
 *         } else {
 *             terminate ("Out of memory");
 *         }
 *     } else {
 *         terminate ("Cannot initialize my_allocator");
 *     }
 *     return 0;
 * }
 *
 *
 * SOURCE
 */
typedef struct allocator_type allocator_type_t;
/****/


/****p* libt7/allocate_allocator_function
 * NAME
 * allocate_allocator_function - allocate allocator structure
 *
 * FUNCTION
 * Prototype of a function who is responsible for allocating memory for an
 * allocator_t structure.  You may need to implement this function in order to
 * build custom memory allocators.  See allocator_type_t for an example.
 *
 * SOURCE
 */
typedef allocator_t *allocate_allocator_function (void);
/****/


/****p* libt7/free_allocator_function
 * NAME
 * free_allocator_function - free allocator
 *
 * FUNCTION
 * Prototype of a function who is responsible for releasing an allocator_t
 * structure.  You may need to implement this function in order to build
 * custom memory allocators.  See allocator_type_t for an example.
 *
 * SOURCE
 */
typedef void free_allocator_function (allocator_t *ap);
/****/


/****p* libt7/create_allocator_function
 * NAME
 * create_allocator_function - create allocator
 *
 * FUNCTION
 * Prototype of a function who is responsible for initializing an allocator_t
 * structure.  You may need to implement this function in order to build
 * custom memory allocators.  See allocator_type_t for an example.
 *
 * SOURCE
 */
typedef int create_allocator_function(
    allocator_t *ap, const allocator_type_t *tp);
/****/


/****p* libt7/destroy_allocator_function
 * NAME
 * destroy_allocator_function - destroy allocator
 *
 * FUNCTION
 * Prototype of a function who is responsible for un-initializing an
 * allocator_t structure.  You may need to implement this function in order to
 * build custom memory allocators.  See allocator_type_t for an example.
 *
 * SOURCE
 */
typedef void destroy_allocator_function (allocator_t *ap);
/****/


/****p* libt7/grab_memory_function
 * NAME
 * grab_memory_function - allocate memory
 *
 * FUNCTION
 * Prototype of a function who is responsible for allocating memory from an
 * allocator.  You may need to define this function when building your own
 * custom memory allocators.
 *
 * SOURCE
 */
typedef void *grab_memory_function (allocator_t *ap, size_t n);
/****/


/****p* libt7/release_memory_function
 * NAME
 * release_memory_function - release memory
 *
 * FUNCTION
 * Prototype of a function who is responsible for releasing memory back to an
 * allocator.  You may need to define this function when building your own
 * custom memory allocators.
 *
 * SOURCE
 */
typedef void release_memory_function (allocator_t *ap, void *p);
/****/


/****p* libt7/resize_memory_function
 * NAME
 * resize_memory_function - prototype of memory resize function
 *
 * FUNCTION
 * Prototype of a function who is responsible for resizing an already
 * allocated memory area.  You may need to define this function when building
 * your own custom memory allocators.
 *
 * SOURCE
 */
typedef void *resize_memory_function (allocator_t *ap, void *p, size_t n);
/****/


/****s* libt7/allocator_type
 * NAME
 * allocator_type - type of memory allocator
 *
 * FUNCTION
 * A structure with pointers to various functions which make up the
 * functionality of a memory allocator.  See allocator_type_t for more
 * information.
 *
 * SOURCE
 */
struct allocator_type {
    allocate_allocator_function *allocate;
    free_allocator_function *free;
    create_allocator_function *create;
    destroy_allocator_function *destroy;
    grab_memory_function *grab;
    release_memory_function *release;
    resize_memory_function *resize;
};
/****/


/****s* libt7/allocator_data
 * NAME
 * allocator_data - memory allocator
 *
 * FUNCTION
 * Structure of a memory allocator.  See allocator_t for more information.
 *
 * SOURCE
 */
struct allocator_data {
    allocator_t *next;
    allocator_t *prev;
    const allocator_type_t *type;
};
/****/


/****v* libt7/default_allocator
 * NAME
 * default_allocator - default memory allocator
 *
 * FUNCTION
 * Pointer to default memory allocator who acquires memory dynamically from
 * the operating system.
 *
 * The pointer to the default memory allocator can also be retrieved with the
 * get_default_allocator function.  However, note that get_default_allocator
 * is affected by fixtures whereas default_allocator variable always points to
 * the same default allocator.
 *
 * EXAMPLE
 * // Get pointer to default allocator unconditionally
 * allocator_t *ap = get_allocator (default_allocator);
 * if (ap) {
 *     // Success
 * } else {
 *     // Failure
 * }
 *
 * SOURCE
 */
extern const allocator_type_t *default_allocator;
/****/


/****f* libt7/get_allocator
 * NAME
 * get_allocator - get pointer to specific allocator
 *
 * FUNCTION
 * Get pointer to the allocator object of type TP.  If the allocator object
 * does not exists, then the function creates a new allocator and returns
 * pointer to that.  Otherwise, the function returns a pointer to previously
 * created allocator object.
 *
 * The function returns NULL if the allocator object cannot be created or the
 * allocator type is invalid.
 *
 * EXAMPLE
 * // Get pointer to static allocator
 * allocator_t *ap = get_allocator (static_allocator);
 * if (ap) {
 *     // Success
 * } else {
 *     // Failure
 * }
 *
 * SYNOPSIS
 */
allocator_t *get_allocator (const allocator_type_t *tp);
/****/


/****f* libt7/get_default_allocator
 * NAME
 * get_default_allocator - get reference to default allocator
 *
 * FUNCTION
 * Get pointer to the default allocator as defined by the active fixture.
 *
 * This function is typically used when an allocator needs to be passed as an
 * argument to another function.  General memory allocation functions such as
 * allocate_memory, free_memory and resize_memory use the default memory
 * allocator by design and are often simpler to use.
 *
 * EXAMPLE
 * // Allocate structure using an optional allocator
 * strut_t *allocate_strut (allocator_t *ap) {
 *     // Use the default allocator if none is provided
 *     if (ap == NULL) {
 *         ap = get_default_allocator ();
 *     }
 *
 *     // Allocate memory from allocator ap
 *     return allocator_allocate_memory (ap, sizeof (strut_t));
 * }
 *
 * SYNOPSIS
 */
allocator_t *get_default_allocator (void);
/****/


/****f* libt7/new_allocator
 * NAME
 * new_allocator - create new allocator
 *
 * FUNCTION
 * Construct new allocator and initialize it with the type TP.  The function
 * returns a pointer to the allocator or NULL if the allocator cannot be
 * created.
 *
 * Use this function to construct short-lived allocators for local use.  Once
 * you don't need the allocator anymore, then be sure to delete it with
 * delete_allocator.
 *
 * EXAMPLE
 * // Construct new static allocator for short-term use
 * allocator_t *ap = new_allocator (static_allocator);
 * if (ap) {
 *
 *     // Allocate memory from static allocator
 *     void *p = allocator_allocate_memory (ap, n);
 *     if (p) {
 *
 *         // Allocation successful, continue processing
 *         ...
 *
 *         // Release memory back to static allocator
 *         allocator_free_memory (ap, p);
 *
 *     } else {
 *         terminate ("Out of memory");
 *     }
 *
 *     // Delete allocator
 *     delete_allocator (ap);
 *
 * } else {
 *     terminate ("Cannot initialize static allocator");
 * }
 *
 * SYNOPSIS
 */
allocator_t *new_allocator (const allocator_type_t *tp);
/****/


/****f* libt7/delete_allocator
 * NAME
 * delete_allocator - delete allocator object
 *
 * FUNCTION
 * Delete allocator AP constructed by new_allocator.
 *
 * Note that any memory allocated from the allocator AP should be explicitly
 * released prior to deleting the allocator: only few allocators release the
 * resources implicitly.
 *
 * SYNOPSIS
 */
void delete_allocator (allocator_t *ap);
/****/


/****f* libt7/allocator_allocate_memory
 * NAME
 * allocator_allocate_memory - allocate memory from allocator
 *
 * FUNCTION
 * Allocate N bytes of memory from the allocator pointed by AP.
 *
 * The function returns a pointer to the newly allocated memory area.  If N is
 * zero or the allocation request cannot be granted, then the function returns
 * NULL.
 *
 * EXAMPLE
 * // Retrieve pointer to custom allocator
 * allocator_t *ap = get_allocator (my_allocator);
 * if (ap) {
 *
 *     // Allocate 1000 bytes from custom allocator
 *     char *p = allocator_allocate_memory (ap, 1000);
 *     if (p) {
 *         // Success
 *     } else {
 *         terminate ("Out of memory");
 *     }
 *
 * } else {
 *     terminate ("Invalid allocator");
 * }
 *
 * SYNOPSIS
 */
void *allocator_allocate_memory (allocator_t *ap, size_t n);
/****/


/****f* libt7/allocator_resize_memory
 * NAME
 * allocator_resize_memory - resize memory using allocator
 *
 * FUNCTION
 * Using allocator AP, attempt to resize the memory area pointed by P to
 * contain at least N bytes.  If the memory area pointed by P cannot be
 * resized, then the function allocates a new memory area, copies the old
 * memory area to the newly allocated memory area and releases the old memory
 * area.
 *
 * The function returns a pointer to the new memory area which replaces the
 * memory area P.  If the new memory request cannot be accommodiated, then the
 * function returns NULL and leaves the memory region pointed by P untouched.
 *
 * Be ware that the contents of the memory area will be unchanged up to the
 * old size but any newly allocated memory needs to be initialized prior to
 * use.
 *
 * SYNOPSIS
 */
void *allocator_resize_memory (allocator_t *ap, void *p, size_t n);
/****/


/****f* libt7/allocator_free_memory
 * NAME
 * allocator_free_memory - release memory to allocator
 *
 * FUNCTION
 * Release memory area pointed by P to the allocator AP.
 *
 * Note that the memory area pointed by the P must have been allocated from
 * the allocator AP or else the program may crash.
 *
 * SYNOPSIS
 */
void allocator_free_memory (allocator_t *ap, void *p);
/****/


/****F* libt7/allocate_allocator
 * NAME
 * allocate_allocator - allocate memory for allocator_t structure
 *
 * FUNCTION
 * Allocate memory for generic allocator_t structure and return pointer to the
 * newly allocated but un-initialized memory area.  If the memory allocation
 * fails, then function return NULL.
 *
 * You can use this function to create custom memory allocators.  However, if
 * the custom memory allocator structure has custom fields, then you will need
 * to allocate the structure with some other function.
 *
 * SYNOPSIS
 */
allocator_t *allocate_allocator (void);
/****/


/****F* libt7/free_allocator
 * NAME
 * free_allocator - release allocator_t structure
 *
 * FUNCTION
 * Release allocator_t structure pointed by AP.  The memory area pointed by AP
 * must have been destroyed prior to release in order to release all the
 * resources held by the allocator.
 *
 * You may need this function for building custom allocators.  However, if the
 * custom allocator structure has custom fields, then you should release the
 * allocator structure with some other function.
 *
 * SYNOPSIS
 */
void free_allocator (allocator_t *ap);
/****/


/****F* libt7/create_allocator
 * NAME
 * create_allocator - initialize allocator object
 *
 * FUNCTION
 * Initialize allocator_t object pointed by AP with the allocator type TP.
 * The function returns true if the allocation is successful.  Otherwise, the
 * function returns zero.
 *
 * Use this function to initialize the base allocator prior to initializing
 * custom fields.
 *
 * SYNOPSIS
 */
int create_allocator (allocator_t *ap, const allocator_type_t *tp);
/****/


/****F* libt7/destroy_allocator
 * NAME
 * destroy_allocator - un-initialize allocator
 *
 * FUNCTION
 * Un-initialize allocator_t structure pointed by AP.  This ensures that all
 * resources held by the allocator are released.
 *
 * Use this function to destroy the base allocator after you have destroyed
 * the custom fields.
 *
 * SYNOPSIS
 */
void destroy_allocator (allocator_t *ap);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_ALLOCATOR_H*/

