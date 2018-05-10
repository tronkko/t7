/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/fixture.h"
#include "t7/simulate-failure.h"
#include "t7/memory.h"

#undef NDEBUG
#include <assert.h>


/* Local functions */
static int allocate (void);
static int sequential (void);
static int handler1 (void);
static int handler2 (void);

/* Code paths taken */
static int path1;
static int path2;
static int path3;
static int path4;


int
main (void)
{
    int ok;

    /* Use the default testing environment this test program */
    set_fixture (test_fixture);

    /* Reset paths */
    path1 = 0;
    path2 = 0;
    path3 = 0;
    path4 = 0;

    /* Execute test function with faulty allocator */
    ok = repeat_test (allocate);
    assert (ok);

    /* Ensure that all code paths were taken */
    assert (path1 == 1);
    assert (path2 == 1);
    assert (path3 == 1);
    assert (path4 == 1);

    /* Reset paths */
    path1 = 0;
    path2 = 0;
    path3 = 0;
    path4 = 0;

    /* Test plain sequential allocation */
    ok = repeat_test (sequential);
    assert (ok);

    /* Ensure that all code paths were taken */
    assert (path1 == 100);
    assert (path2 == 0);
    assert (path3 == 0);
    assert (path4 == 0);

    /* Reset paths */
    path1 = 0;
    path2 = 0;
    path3 = 0;
    path4 = 0;

    /* Test simple error handling branches */
    ok = repeat_test (handler1);
    assert (ok);

    /* Ensure that all code paths were taken */
    assert (path1 == 1);
    assert (path2 == 1);
    assert (path3 == 1);
    assert (path4 == 1);

    /* Reset paths */
    path1 = 0;
    path2 = 0;
    path3 = 0;
    path4 = 0;

    /* Test complex error handling */
    ok = repeat_test (handler2);
    assert (ok);

    /* Ensure that all code paths were taken */
    assert (path1 == 1);
    assert (path2 == 1);
    assert (path3 == 1);
    assert (path4 == 1);

    return 0;
}


/* Test memory allocation functions */
static int
allocate (void)
{
    int ok;
    unsigned char *p;
    unsigned char *q;
    unsigned i;

    /* Try to allocate 100 bytes */
    p = allocate_memory (100);
    if (p) {

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

        /* Enlarge memory region */
        q = resize_memory (p, 10000);
        if (q) {

            /* Allocation successful */
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
            if (q) {

                /* Got more memory */
                p = q;

                /* Make sure that contents don't change */
                for (i = 0; i < 10000; i++) {
                    assert (p[i] == 0x00);
                }

                /* Everything worked OK */
                ok = 1;
                path4++;

            } else {

                /* Cannot enlarge for the second time */
                ok = 0;
                path3++;

            }

        } else {

            /* Cannot enlarge for the first time */
            ok = 0;
            path2++;

        }

        /* Release memory */
        free_memory (p);

    } else {

        /* Initial allocation failed */
        ok = 0;
        path1++;

    }
    return ok;
}


/* Test sequential memory allocation */
static int
sequential (void)
{
    void *arr[100];
    int i;
    int ok = 1;

    /* Allocate 100 blocks of memory */
    i = 0;
    while (i < 100) {

        /* Allocate one block and break out on failure */
        arr[i] = allocate_memory (100);
        if (arr[i] == NULL) {
            path1++;
            ok = 0;
            break;
        }

        /* Allocate next block */
        i++;

    }

    /* Release allocated blocks */
    while (i > 0) {
        free_memory (arr[--i]);
    }

    return ok;
}


/* Test error handling */
static int
handler1 (void)
{
    int ok;
    unsigned char *p;
    unsigned char *q;

    /* Try to allocate 100 bytes */
    p = allocate_memory (100);
    if (p) {

        /* Enlarge memory region */
        q = resize_memory (p, 10000);
        if (q) {

            /* Allocation successful */
            p = q;

            /* Everything worked OK */
            ok = 1;
            path4++;

        } else {

            /* Cannot enlarge for the first time */
            ok = 0;
            path3++;

        }

    } else {

        /* Initial allocation failed, attempt to handle error */
        p = allocate_memory (10);
        if (p) {

            /* Success, error resolved */
            path2++;
            ok = 1;

        } else {

            /* Second allocation failed too */
            ok = 0;
            path1++;

        }

    }

    /* Release memory */
    if (p) {
        free_memory (p);
    }
    return ok;
}


/* Test complex error handling */
static int
handler2 (void)
{
    int ok;
    unsigned char *p;

    /* Try to allocate 1000 bytes */
    p = allocate_memory (1000);
    if (p) {

        /* First allocation succeeded */
        ok = 1;
        path4++;

    } else {

        /* First allocation failed, attempt to handle error */
        p = allocate_memory (100);
        if (p) {

            /* Second allocation succeeded */
            path3++;
            ok = 1;

        } else {

            /* Second allocation failed too, attempt to handle that */
            p = allocate_memory (10);
            if (p) {

                /* Third allocation succeeded */
                ok = 1;
                path2++;

            } else {

                /* Third allocation failed */
                ok = 0;
                path1++;

            }

        }

    }

    /* Release memory */
    if (p) {
        free_memory (p);
    }
    return ok;
}



