/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_THREAD_H
#define T7_THREAD_H
#ifdef __cplusplus
extern "C" {
#endif


/* Forward-decl */
struct thread_data;
struct thread_type;


/****t* libt7/thread_t
 * NAME
 * thread_t - thread of execution
 *
 * FUNCTION
 * A structure containing data for concurrently running thread of execution.
 * See thread_data for list of fields.
 *
 * SOURCE
 */
typedef struct thread_data thread_t;
/****/


/****t* libt7/thread_type_t
 * NAME
 * thread_type_t - type of thread
 *
 * FUNCTION
 * A structure containing pointer to various functions which define the
 * functionality of the thread.  See thread_type for list of functions.
 *
 * You will need to define a variable of this type in order to run concurrent
 * threads.  See below for an example.
 *
 * EXAMPLE
 * // Run a custom thread
 * #include "t7/thread.h"
 * #include "t7/terminate.h"
 *
 * // Prototype of a threaded function
 * static int my_run (thread_t *tp);
 *
 * // Define basic thread type
 * static thread_type_t def1 = {
 *     allocate_thread,
 *     free_thread,
 *     create_thread,
 *     destroy_thread,
 *     my_run
 * };
 *
 * // Shorthand for thread type
 * static thread_type_t *my_thread = &def1;
 *
 * // The main program
 * int main (void) {
 *     thread_t *tp;
 *     int result;
 *
 *     // Create basic thread
 *     tp = new_thread (my_thread);
 *     if (tp) {
 *
 *         // Start the thread
 *         result = start_thread (tp);
 *         if (result) {
 *
 *             // Wait for thread to finish
 *             result = join_thread (tp);
 *
 *         } else {
 *             terminate ("Cannot start thread");
 *         }
 *
 *         // Release resources
 *         delete_thread (tp);
 *
 *     } else {
 *         terminate ("Cannot create thread");
 *     }
 *     return 0;
 * }
 *
 * // The threaded function
 * static int my_run (thread_t *tp) {
 *     // Execute some task
 *     ...
 *     return 1;
 * }
 *
 * SOURCE
 */
typedef struct thread_type thread_type_t;
/****/


/****p* libt7/allocate_thread_function
 * NAME
 * allocate_thread_function - allocate thread_t structure
 *
 * FUNCTION
 * A prototype of a function who is responsible for allocating memory for
 * thread_t structure.  If the allocation is successful, then the function
 * must return a pointer to the newly allocated thread_t structure.
 * Otherwise, the function must return NULL.
 *
 * You will need to implement this function if your thread needs custom
 * fields.  Otherwise, you can use the function allocate_thread in your custom
 * thread_type_t structure.
 *
 * SOURCE
 */
typedef thread_t *allocate_thread_function (void);
/****/


/****p* libt7/free_thread_function
 * NAME
 * free_thread_function - release thread_t structure
 *
 * FUNCTION
 * A prototype of a function who is responsible for releasing memory held by
 * thread_t structure.
 *
 * You may need to implement this function if your thread needs custom fields.
 * Otherwise, you can use the function free_thread in your custom
 * thread_type_t structure.
 *
 * SOURCE
 */
typedef void free_thread_function (thread_t *tp);
/****/


/****p* libt7/create_thread_function
 * NAME
 * create_thread_function - initialize thread
 *
 * FUNCTION
 * A prototype of a function who is responsible for initializing fields of the
 * thread_t structure.
 *
 * You may need to implement this function if your thread needs custom fields.
 * Otherwise, you can use the function create_thread in your custom
 * thread_type_t structure.
 *
 * SOURCE
 */
typedef int create_thread_function (thread_t *tp, const thread_type_t *typ);
/****/


/****p* libt7/destroy_thread_function
 * NAME
 * destroy_thread_function - un-initialize thread
 *
 * FUNCTION
 * A prototype of a function who is responsible for un-initializing fields of
 * the thread_t structure.
 *
 * You may need to implement this function if your thread needs custom fields.
 * Otherwise, you can use the function destroy_thread in your custom
 * thread_type_t structure.
 *
 * SOURCE
 */
typedef void destroy_thread_function (thread_t *tp);
/****/


/****p* libt7/run_thread_function
 * NAME
 * run_thread_function - execute thread
 *
 * FUNCTION
 * A prototype of a function who is responsible for doing some threaded task.
 * The function should return a non-zero status on success and zero on
 * failure.
 *
 * SOURCE
 */
typedef int run_thread_function (thread_t *tp);
/****/


/****s* libt7/thread_type
 * NAME
 * thread_type - type of thread
 *
 * FUNCTION
 * A structure containing pointer to various functions which define the
 * functionality of the thread.  See thread_type_t for more information.
 *
 * SOURCE
 */
struct thread_type {
    allocate_thread_function *allocate;
    free_thread_function *free;
    create_thread_function *create;
    destroy_thread_function *destroy;
    run_thread_function *run;
};
/****/


/****s* libt7/thread_data
 * NAME
 * thread_data - thread of execution
 *
 * FUNCTION
 * A structure containing data for concurrently running thread of execution.
 * See thread_t for more information.
 *
 * SOURCE
 */
struct thread_data {
    const thread_type_t *type;
    void *impl;
};
/****/


/****f* libt7/has_threads
 * NAME
 * has_threads - returns true if threads are enabled
 *
 * FUNCTION
 * Returns true if support for multiple concurrent threads has been compiled
 * in.
 *
 * SOURCE
 */
int has_threads (void);
/****/


/****f* libt7/new_thread
 * NAME
 * new_thread - construct a new thread
 *
 * FUNCTION
 * Allocate new thread object and initialize it with type TYP.
 *
 * The function returns a pointer to newly initialized thread structure.  This
 * structure must be released with delete_thread once the thread is no
 * longer needed.
 *
 * See thread_t for an example on how to create custom threads.
 *
 * SYNOPSIS
 */
thread_t *new_thread (const thread_type_t *typ);
/****/


/****f* libt7/delete_thread
 * NAME
 * delete_thread - delete thread object
 *
 * FUNCTION
 * Release resources held by the thread TP.  If the thread is running at the
 * time of the function call, then the thread will be cancelled.
 *
 * SYNOPSIS
 */
void delete_thread (thread_t *tp);
/****/


/****f* libt7/start_thread
 * NAME
 * start_thread - start concurrent thread
 *
 * FUNCTION
 * Start running thread TP concurrently.  If the thread was started
 * successfully, then the function returns true.  Otherwise, the function
 * returns false.
 *
 * SYNOPSIS
 */
int start_thread (thread_t *tp);
/****/


/****f* libt7/join_thread
 * NAME
 * join_thread - wait for thread to finish
 *
 * FUNCTION
 * Wait for thread TP to finish and return the exit status of the thread.  If
 * the thread TP is invalid or cannot be joined, then the function returns
 * zero.
 *
 * Be ware that exit status of zero may be returned by either an error joining
 * to the thread or the thread function itself.
 *
 * SYNOPSIS
 */
int join_thread (thread_t *tp);
/****/


/****f* libt7/yield
 * NAME
 * yield - yield the processor
 *
 * FUNCTION
 * Yield the processor for other threads to use.
 *
 * Use this function when the current thread has released locks and needs to
 * wait for other threads to produce more data for processing.
 *
 * SYNOPSIS
 */
void yield (void);
/****/


/****F* libt7/create_thread
 * NAME
 * create_thread - initialize thread
 *
 * FUNCTION
 * Initialize thread at memory area pointed by TP using thread type TYP.  The
 * function returns a non-zero number if the initialization succeeded and zero
 * otherwise.
 *
 * You will typically use this function in your custom thread_type_t
 * structure or custom create_thread_function.
 *
 * SYNOPSIS
 */
int create_thread (thread_t *tp, const thread_type_t *typ);
/****/


/****F* libt7/destroy_thread
 * NAME
 * destroy_thread - un-initialize thread
 *
 * FUNCTION
 * Un-initialize thread pointed by TP and release all resources associated
 * with it.
 *
 * You will typically use this function in your custom thread_type_t
 * structure or custom destroy_thread_function.
 *
 * SYNOPSIS
 */
void destroy_thread (thread_t *tp);
/****/


/****F* libt7/allocate_thread
 * NAME
 * allocate_thread - allocate memory for generic thread structure
 *
 * FUNCTION
 * Allocate thread_t structure and return pointer to un-initialized memory
 * area.  If the structure cannot be allocated, then the function returns
 * NULL.
 *
 * You will typically use this function in your custom thread_type_t
 * structure or custom allocate_thread_function.
 *
 * SYNOPSIS
 */
thread_t *allocate_thread (void);
/****/


/****F* libt7/free_thread
 * NAME
 * free_thread - release thread structure
 *
 * FUNCTION
 * Release memory area allocated by allocate_thread().
 *
 * You will typically use this function in your custom thread_type_t
 * structure or custom free_thread_function.
 *
 * SYNOPSIS
 */
void free_thread (thread_t *tp);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_THREAD_H*/
