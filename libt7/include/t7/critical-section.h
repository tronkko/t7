/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_CRITICAL_H
#define T7_CRITICAL_H
#ifdef __cplusplus
extern "C" {
#endif


/****f* libt7/enter_critical
 * NAME
 * enter_critical - enter critical section
 *
 * FUNCTION
 * Enter critical section of code which may be executed by only one thread at
 * a time.  Once the critial section of code has been executed, be sure to
 * leave the critical section by calling leave_critical!  Failing to exit from
 * a critical section, for example by returning prematurely from a function,
 * will cripple most applications!
 *
 * Be ware that enter_critical will terminate the current application if the
 * critical section cannot be preserved for the current thread.
 *
 * EXAMPLE
 * // Multi-thread safe function
 * static int get_slot (void) {
 *     int result = -1;
 *
 *     // Lock out other threads
 *     enter_critical ();
 *
 *     // Allocate first free slot
 *     for (i = 0; i < 100; i++) {
 *         if (slot[i].allocated == 0) {
 *             // Allocate slot
 *             slot[i].allocated = 1;
 *             result = i;
 *         }
 *     }
 *
 *     // Allow other threads to access data
 *     leave_critical ();
 *
 *     // Return allocated slot or -1 to signal error
 *     return result;
 * }
 *
 * SYNOPSIS
 */
void enter_critical (void);
/****/


/****f* libt7/leave_critical
 * NAME
 * leave_critical - leave critical section
 *
 * FUNCTION
 * Leave critical section of code and allow other threads to access it.  See
 * enter_critical for an example.
 *
 * Be ware that leave_critical will not return in case of error.
 *
 * SYNOPSIS
 */
void leave_critical (void);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_CRITICAL_H*/
