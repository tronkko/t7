/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/memory.h"

#undef NDEBUG
#include <assert.h>


int
main (void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned i;

    /* Allocate 100 bytes */
    p = allocate_memory (100);
    assert (p != NULL);

    /* Fill memory with zero and make sure memory was cleared */
    zero_memory (p, 100);
    for (i = 0; i < 100; i++) {
        assert (p[i] == 0);
    }

    /* Fill memory with 0xCC and make sure memory was set */
    fill_memory (p, 0xCC, 100);
    for (i = 0; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Filling zero bytes changes nothing */
    fill_memory (p, 0xFF, 0);
    for (i = 0; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Filling one byte affects only one byte */
    fill_memory (p, 0xFF, 1);
    assert (p[0] == 0xFF);
    for (i = 1; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Zeroing one byte affects only one byte, second time */
    zero_memory (p, 1);
    assert (p[0] == 0x00);
    for (i = 1; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Fill two bytes */
    fill_memory (p, 0xEE, 2);
    assert (p[0] == 0xEE);
    assert (p[1] == 0xEE);
    for (i = 2; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Copying one byte affects the target byte only */
    copy_memory (p + 3, p, 1);
    assert (p[0] == 0xEE);
    assert (p[1] == 0xEE);
    assert (p[2] == 0xCC);
    assert (p[3] == 0xEE);
    for (i = 4; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Moving bytes */
    move_memory (p, p + 1, 4);
    assert (p[0] == 0xEE);
    assert (p[1] == 0xCC);
    assert (p[2] == 0xEE);
    assert (p[3] == 0xCC);
    for (i = 4; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Swap bytes */
    swap_memory (p, p + 1, 1);
    assert (p[0] == 0xCC);
    assert (p[1] == 0xEE);
    assert (p[2] == 0xEE);
    assert (p[3] == 0xCC);
    for (i = 4; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Enlarge memory */
    q = resize_memory (p, 10000);
    assert (q != NULL);
    p = q;

    /* Make sure that contents stay the same */
    assert (p[0] == 0xCC);
    assert (p[1] == 0xEE);
    assert (p[2] == 0xEE);
    assert (p[3] == 0xCC);
    for (i = 4; i < 100; i++) {
        assert (p[i] == 0xCC);
    }

    /* Fill memory area */
    zero_memory (p, 10000);

    /* Enlarge again */
    q = resize_memory (p, 100000);
    assert (q != NULL);
    p = q;

    /* Make sure that contents don't change */
    for (i = 0; i < 10000; i++) {
        assert (p[i] == 0x00);
    }

    /* Release memory */
    free_memory (p);
    return 0;
}


