/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_SIMULATE_FAILURE_H
#define T7_SIMULATE_FAILURE_H
#ifdef __cplusplus
extern "C" {
#endif


/****p* libt7/test_function
 * NAME
 * test_function - prototype of a testable function
 *
 * FUNCTION
 * Prototype of a function who is responsible for executing a test and
 * returning true if the test passes.  See repeat_test for more information.
 *
 * SOURCE
 */
typedef int test_function (void);
/****/


/****f* libt7/simulate_failure
 * NAME
 * simulate_failure - returns true when failure should be simulated
 *
 * FUNCTION
 * Returns true when testing is enabled and the caller should simulate a
 * failure by returning some error status.
 *
 * EXAMPLE
 * // Read a file
 * static int read (const char *fn) {
 *     FILE *fp;
 *     int ok;
 *
 *     // Open file with possibly simulated failure
 *     if (!simulate_failure ()) {
 *
 *         // Open file for real
 *         fp = fopen (fn, "r");
 *         if (fp) {
 *             // File opened OK
 *             ...
 *             ok = 1;
 *         } else {
 *             // Real failure
 *             ok = 0;
 *         }
 *
 *     } else {
 *         // Simulated failure
 *         ok = 0;
 *     }
 *     return fp;
 * }
 *
 * SYNOPSIS
 */
int simulate_failure (void);
/****/


/****f* libt7/repeat_test
 * NAME
 * repeat_test - branch and execute a test function repeatedly
 *
 * FUNCTION
 * Execute the test_function F with failure simulation enabled.  Due to
 * failure simulation, the test_function will be executed repeatedly with
 * different points of failure.
 *
 * The repeat_test function is often used with test_fixture, which modifies
 * the allocate_memory and resize_memory function to fail when
 * simulate_failure() returns true.
 *
 * EXAMPLE
 * #include "t7/fixture.h"
 * #include "t7/simulate-failure.h"
 * #include "t7/memory.h"
 * #include <assert.h>
 *
 * // Prototype of a test function
 * static int allocate (void);
 *
 * // Main program
 * int main (void) {
 *     // Set up testing environment
 *     set_fixture (test_fixture);
 *
 *     // Execute the test function twice: once such that the allocate_memory
 *     // fails and once such that allocate_memory succeds.
 *     ok = repeat_test (allocate);
 *
 *     // Make sure that test function returned true
 *     assert (ok);
 *     return 0;
 * }
 *
 * // The test function
 * static int allocate (void) {
 *     int ok;
 *     unsigned char *p;
 *
 *     // Try to allocate 100 bytes of memory
 *     p = allocate_memory (100);
 *     if (p) {
 *         // Allocation succeeded
 *         free_memory (p);
 *         ok = 1;
 *     } else {
 *         // Failed to allocate memory
 *         ok = 0;
 *     }
 *     return ok;
 * }
 *
 * SYNOPSIS
 */
int repeat_test (test_function *f);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_SIMULATE_FAILURE_H*/

