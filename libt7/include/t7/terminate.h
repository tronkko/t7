/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_TERMINATE_H
#define T7_TERMINATE_H
#ifdef __cplusplus
extern "C" {
#endif


/****f* libt7/terminate
 * NAME
 * terminate - stop application with error message
 *
 * FUNCTION
 * Output character string pointed by PC to system log and stop the
 * current program.
 *
 * EXAMPLE
 * // Attempt to allocate some memory
 * char *p = allocate_memory (100);
 * if (p) {
 *     // Success
 * } else {
 *     terminate ("Out of memory");
 * }
 * SYNOPSIS
 */
void terminate (const char *pc);
/****/


/****f* libt7/exit_application
 * NAME
 * exit_application - exit from application
 *
 * FUNCTION
 * Exit from application with the exit status STATUS.
 *
 * SYNOPSIS
 */
void exit_application (int status);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_TERMINATE_H*/
