/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_TLS_H
#define T7_TLS_H
#ifdef __cplusplus
extern "C" {
#endif


/* Forward-decl */
struct tls_type;
struct tls_data;


/****t* libt7/tls_type_t
 * NAME
 * tls_type_t - type of thread-local variable
 *
 * FUNCTION
 * Structure with pointers to various functions which define the functionality
 * of the thread-local variable.  See tls_type for a list of functions.
 *
 * You will need to define a structure of this type in order to construct
 * thread-local variables of your own.  See below for an example.
 *
 * EXAMPLE
 * // Create custom variable
 * #include "t7/tls.h"
 *
 * // Prototypes
 * static tls_variable_t *allocate_dummy (void);
 * static void free_dummy (tls_variable_t *vp);
 * static int create_dummy (tls_variable_t *vp, const tls_type_t *tp);
 * static void destroy_dummy (tls_variable_t *vp);
 * static void *get_dummy (tls_variable_t *vp);
 *
 * // Define custom structure for the thread-local data
 * struct dummy {
 *     // Base variable, must be the first member of the structure
 *     tls_variable_t base;
 *
 *     // Custom data
 *     int value;
 * };
 *
 * // Define custom variable type
 * static tls_type_t mytp1 = {
 *     allocate_dummy,
 *     free_dummy,
 *     create_dummy,
 *     destroy_dummy,
 *     get_dummy,
 * };
 *
 * // Define shorthand for the type
 * static tls_type_t *dummy_variable = &mytp1;
 *
 * // Allocate memory for variable
 * static tls_variable_t *allocate_dummy (void) {
 *     return (tls_variable_t*) allocate_memory (sizeof (struct dummy));
 * }
 *
 * // Release variable
 * static void free_dummy (tls_variable_t *vp) {
 *     free_memory (vp);
 * }
 *
 * // Initialize variable with zero value
 * static int create_dummy (tls_variable_t *vp, const tls_type_t *tp) {
 *     int ok;
 *
 *     // Initialize base variable
 *     if (create_tls (vp, tp)) {
 *
 *         // Initialize custom data
 *         struct dummy *dp = (struct dummy*) vp;
 *         dp->value = 0;
 *
 *         // Success
 *         ok = 1;
 *
 *     } else {
 *
 *        // Cannot initialize base variable
 *         ok = 0;
 *
 *     }
 *     return ok;
 * }
 *
 * // Un-initialize variable
 * static void destroy_dummy (tls_variable_t *vp) {
 *     // Destroy base variable
 *     destroy_tls (vp);
 * }
 *
 * // Get address of custom data
 * static void *get_dummy (tls_variable_t *vp) {
 *     struct dummy *dp = (struct dummy*) vp;
 *     return &dp->value;
 * }
 *
 * // Main program
 * int main (void) {
 *     int *p;
 *
 *     // Retrieve pointer to custom variable (now initialized with zero)
 *     p = get_tls (dummy_variable);
 *
 *     // Set variable to 13
 *     *p = 13;
 *
 *     return 0;
 * }
 *
 * SOURCE
 */
typedef struct tls_type tls_type_t;
/****/


/****t* libt7/tls_variable_t
 * NAME
 * tls_variable_t - thread-local variable
 *
 * FUNCTION
 * A structure containing data for a thread-local variable.  See
 * tls_data for a list of fields.
 *
 * You will need to define a structure of this type in order to construct
 * thread-local variables of your own.  See tls_type_t for an example.
 *
 * SOURCE
 */
typedef struct tls_data tls_variable_t;
/****/


/****p* libt7/allocate_tls_function
 * NAME
 * allocate_tls_function - allocate thread-local variable
 *
 * FUNCTION
 * Prototype of a function who is responsible for allocating memory for
 * tls_variable_t structure.  You will need to implement this function in
 * order to define your own thread-local variables.  See tls_type_t for an
 * example.
 *
 * SOURCE
 */
typedef tls_variable_t *allocate_tls_function (void);
/****/


/****p* libt7/free_tls_function
 * NAME
 * free_tls_function - release thread-local variable
 *
 * FUNCTION
 * Prototype of a function who is responsible for releasing tls_variable_t
 * structure.  You will need to implement this function in order to define
 * your own thread-local variables.  See tls_type_t for an example.
 *
 * SOURCE
 */
typedef void free_tls_function (tls_variable_t *vp);
/****/


/****p* libt7/create_tls_function
 * NAME
 * create_tls_function - initialize thread-local variable
 *
 * FUNCTION
 * Prototype of a function who is responsible for initializing tls_variable_t
 * structure.  You will need to implement this function in order to define
 * your own thread-local variables.  See tls_type_t for an example.
 *
 * SOURCE
 */
typedef int create_tls_function (tls_variable_t *vp, const tls_type_t *tp);
/****/


/****p* libt7/destroy_tls_function
 * NAME
 * destroy_tls_function - un-initialize thread-local variable
 *
 * FUNCTION
 * Prototype of a function who is responsbile for un-initializing
 * tls_variable_t structure.  You will need to implement this function in
 * order to define your own thread-local variables.  See tls_type_t for an
 * example.
 *
 * SOURCE
 */
typedef void destroy_tls_function (tls_variable_t *vp);
/****/


/****p* libt7/get_tls_function
 * NAME
 * get_tls_function - get pointer to thread-local data
 *
 * FUNCTION
 * Prototype of a function who is responsible for returning a pointer to
 * actual thread-local data.  You will need to implement this function in
 * order to define your own thread-local variables.  See tls_type_t for an
 * example.
 *
 * SYNOPSIS
 */
typedef void *get_tls_function (tls_variable_t *vp);
/****/


/****s* libt7/tls_type
 * NAME
 * tls_type - type of thread-local variable
 *
 * FUNCTION
 * Structure with pointers to various functions which define the functionality
 * of the thread-local variable.  See tls_type_t for more information.
 *
 * SOURCE
 */
struct tls_type {
    allocate_tls_function *allocate;
    free_tls_function *free;
    create_tls_function *create;
    destroy_tls_function *destroy;
    get_tls_function *get;
};
/****/


/****s* libt7/tls_data
 * NAME
 * tls_data - thread-local data
 *
 * FUNCTION
 * A structure containing data for a thread-local variable.  See
 * tls_variable_t for more information.
 *
 * SOURCE
 */
struct tls_data {
    tls_variable_t *next;
    const tls_type_t *type;
};
/****/


/****f* libt7/get_tls
 * NAME
 * get_tls - get value of thread-local variable
 *
 * FUNCTION
 * Get pointer to thread-local variable.
 *
 * Be ware that the actual data type of the returned pointer depends on the
 * get_tls_function.  Typically, you will need cast the value to the proper
 * data type before you can use the value.
 *
 * SYNOPSIS
 */
void *get_tls (const tls_type_t *tp);
/****/


/****F* libt7/create_tls
 * NAME
 * create_tls - initialize thread-local variable
 *
 * FUNCTION
 * Initialize thread-local variable pointed by VP with the type TP and return
 * true if the initialization succeeds.  You will typically call this function
 * from your custom create_tls_function.
 *
 * EXAMPLE
 * // Create custom variable
 * static int create_custom (tls_variable_t *vp, const tls_type_t *tp) {
 *     int ok;
 *
 *     // Initialize base variable
 *     if (create_tls (vp, tp)) {
 *
 *         // Convert pointer to custom variable
 *         struct custom_tls_variable *dp = (struct custom_tls_variable*) vp;
 *
 *         // Initialize custom fields
 *         dp->data = NULL;
 *
 *         // Success
 *         ok = 1;
 *
 *     } else {
 *
 *         // Cannot initialize base variable
 *         ok = 0;
 *
 *     }
 *     return ok;
 * }
 *
 * SYNOPSIS
 */
int create_tls (tls_variable_t *vp, const tls_type_t *tp);
/****/


/****F* libt7/destroy_tls
 * NAME
 * destroy_tls - un-initialize thread-local variable
 *
 * FUNCTION
 * Destroy thread-local variable pointed by VP.  You will typically call this
 * function from your custom destroy_tls_function.
 *
 * EXAMPLE
 * // Un-initialize dummy TLS variable
 * static void destroy_dummy (tls_variable_t *vp) {
 *     // Convert pointer to custom variable
 *     struct custom_tls_variable *dp = (struct custom_tls_variable*) vp;
 *
 *     // Destroy custom fields
 *     if (dp->data) {
 *         free_memory (dp->data);
 *         dp->data = NULL;
 *     }
 *
 *     // Destroy base variable
 *     destroy_tls (vp);
 * }
 *
 * SYNOPSIS
 */
void destroy_tls (tls_variable_t *vp);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_TLS_H*/

