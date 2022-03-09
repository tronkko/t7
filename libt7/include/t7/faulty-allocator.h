/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_FAULTY_ALLOCATOR_H
#define T7_FAULTY_ALLOCATOR_H
#include "t7/allocator.h"
#include "t7/fixture.h"
#ifdef __cplusplus
extern "C" {
#endif


/* Faulty allocator type */
extern const struct allocator_vtable *faulty_allocator;

/* For defining custom fixtures */
static struct allocator *get_faulty_allocator(fixture_t *fp);


#ifdef __cplusplus
}
#endif
#endif /*T7_FAULTY_ALLOCATOR_H*/

