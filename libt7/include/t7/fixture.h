/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_FIXTURE_H
#define T7_FIXTURE_H
#include "t7/allocator.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Forward-decl */
struct fixture_data;

/****t* libt7/fixture_t
 * NAME
 * fixture_t - execution environment
 *
 * FUNCTION
 * A structure with a collection of functions which let the program access the
 * execution environment in an operating-system independent way.  See
 * fixture_data for list of functions.
 *
 * Most functions in the T7 library use fixtures implicitly.  If you ever need
 * to access the execution environment through a fixture, then use the
 * function get_fixture to retrieve a pointer to the currently active
 * fixture_t structure.
 *
 * EXAMPLE
 * // Define custom fixture
 * #include "t7/fixture.h"
 * #include "t7/allocator.h"
 *
 * // Prototype of custom get_fixture_allocator_function
 * static allocator_t *get_my_allocator (fixture_t *fp);
 *
 * // Define custom fixture
 * static fixture_t def1 = {
 *     get_my_allocator
 * };
 *
 * // Define shorthand for fixture
 * static fixture_t *my_fixture = &def1;
 *
 * // Main program
 * int main (void) {
 *     // Start using the custom fixture
 *     set_fixture (my_fixture);
 *
 *     // Execute program with modified environment
 *     ...
 *
 *     return 0;
 * }
 *
 * // Implement custom get_fixture_allocator_function
 * static allocator_t *get_my_allocator (fixture_t *fp) {
 *     (void) fp;
 *
 *     // All memory allocations take place in one-megabyte buffer
 *     return get_allocator (static_allocator);
 * }
 *
 * SOURCE
 */
typedef struct fixture_data fixture_t;
/****/


typedef struct allocator *get_fixture_allocator_function(fixture_t *fp);


/****s* libt7/fixture_data
 * NAME
 * fixture_data - execution environment
 *
 * FUNCTION
 * A structure with a collection of functions which let the program access the
 * execution environment in an operating-system independent way.  See
 * fixture_t for more information.
 *
 * SOURCE
 */
struct fixture_data {
	get_fixture_allocator_function *get_fixture_allocator;
};
/****/


/****v* libt7/default_fixture
 * NAME
 * default_fixture - default execution environment
 *
 * FUNCTION
 * Pointer to the default fixture which provides unrestricted access to the
 * execution environment.
 *
 * SOURCE
 */
extern fixture_t *default_fixture;
/****/


/****v* libt7/test_fixture
 * NAME
 * test_fixture - default testing environment
 *
 * FUNCTION
 * Pointer to a fixture which has been optimized for testing.  In this
 * fixture, memory allocation functions have been changed to support
 * simulation of failures.  See repeat_test for more information.
 *
 * SOURCE
 */
extern fixture_t *test_fixture;
/****/


/****f* libt7/get_fixture
 * NAME
 * get_fixture - get current fixture
 *
 * FUNCTION
 * Retrieve pointer to current fixture.
 *
 * If a fixture has already been created, then the function returns a pointer
 * to the previously created fixture.  Otherwise, the function creates a new
 * fixture and returns pointer to that.  If the fixture cannot be created,
 * then the function terminates the program with an error message.
 *
 * SYNOPSIS
 */
fixture_t *get_fixture (void);
/****/


/****f* libt7/set_fixture
 * NAME
 * set_fixture - set active fixture
 *
 * FUNCTION
 * Set the active fixture that is used to communicate with the execution
 * environment.  The T7 library provides ready-made fixtures default_fixture
 * and test_fixture for you.  You can also define custom fixtures.  See
 * fixture_t for an example.
 *
 * Be ware that fixture should be set very early in the program run.  Changing
 * fixtures carelessly at the middle of the run may lead to unexpected
 * consequences!
 *
 * Also be ware that the fixture set by this function will be used for the
 * current thread as well as any sub-threads that the current thread creates.
 * However, existing threads will not be affected.
 *
 * The fixture object pointed by the FP must be held available until the
 * current thread is destroyed or the fixture is changed.
 *
 * EXAMPLE
 * #include "t7/fixture.h"
 * #include "t7/simulate-failure.h"
 *
 * int main (void) {
 *     // Set up test environment
 *     set_fixture (test_fixture);
 *
 *     // Execute tests
 *     ...
 *
 *     return 0;
 * }
 *
 * SYNOPSIS
 */
void set_fixture (fixture_t *fp);
/****/


/****f* libt7/copy_fixture
 * NAME
 * copy_fixture - copy fixture data
 *
 * FUNCTION
 * Overwrite fixture pointed by DEST with the fixture pointed by SRC.
 *
 * SYNOPSIS
 */
void copy_fixture (fixture_t *dest, const fixture_t *src);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_FIXTURE_H*/
