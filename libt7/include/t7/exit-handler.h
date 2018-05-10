/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_EXIT_HANDLER_H
#define T7_EXIT_HANDLER_H
#ifdef __cplusplus
extern "C" {
#endif

/****p* libt7/exit_function
 * NAME
 * exit_function - prototype of exit function
 *
 * FUNCTION
 * Prototype of a function who's purpose is to clean up before the application
 * closes.  See exit_handler for an example.
 *
 * SOURCE
 */
typedef void exit_function (void);
/****/


/****f* libt7/exit_handler
 * NAME
 * exit_handler - register function to be called at program exit
 *
 * FUNCTION
 * Register function F to be called at the program exit.  The argument
 * PRIORITY specifies the priority of the function with respect to other exit
 * functions: exit functions with higher priorities will be called before
 * functions of lower priorities.
 *
 * The following priorities are used at the moment:
 *
 *     Priority | Objects destroyed
 *     ---------+-------------------------------------------------------------
 *     40       | Thread-local storage
 *     30       | Fixtures
 *     20       | Allocators
 *     10       | Critical sections
 *
 * Custom functions should use priorities of 100 and above to ensure that
 * system is fully functional when the exit function is called.
 *
 * The function returns true if the exit function was registered properly.
 * Otherwise, the function returns zero.  The function is known to return zero
 * if more than 32 exit handlers are registered, or the function F has already
 * been registered.
 *
 * EXAMPLE
 * // Local variable
 * static void *pdata = NULL;
 *
 * // Define a custom exit_function
 * static void my_exit (void) {
 *     free_memory (pdata);
 *     pdata = NULL;
 * }
 *
 * // Get pointer to memory buffer
 * void *get_buffer () {
 *     // Allocate buffer at first time
 *     if (pdata == NULL) {
 *         // Allocate data
 *         pdata = allocate_memory (1024*1024);
 *
 *         // Release the memory prior to exiting the program
 *         exit_handler (my_exit, 100);
 *     }
 *
 *     // Return previously allocated buffer
 *     return pdata;
 * }
 *
 * SYNOPSIS
 */
int exit_handler (exit_function *f, int priority);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_EXIT_HANDLER_H*/
