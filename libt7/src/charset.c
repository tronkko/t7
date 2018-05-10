/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/charset.h"
#include "t7/terminate.h"


/* Parser modes for parse_charset() */
enum parse_mode {
    PARSE_INIT = 0,
    PARSE_A = 1,
    PARSE_AS = 2,
    PARSE_ASC = 3,
    PARSE_ASCI = 4,
    PARSE_I = 5,
    PARSE_IS = 6,
    PARSE_ISO = 7,
    PARSE_ISO8 = 8,
    PARSE_ISO88 = 9,
    PARSE_ISO885 = 10,
    PARSE_ISO8859 = 11,
    PARSE_ISO8859DASH = 12,
    PARSE_L = 14,
    PARSE_LA = 15,
    PARSE_LAT = 16,
    PARSE_LATI = 17,
    PARSE_LATIN = 18,
    PARSE_U = 19,
    PARSE_UT = 20,
    PARSE_UTF = 21,
    PARSE_UTF1 = 22,
    PARSE_UTF16 = 23,
    PARSE_UTF16B = 24,
    PARSE_UTF16L = 25,
    PARSE_UTF3 = 26,
    PARSE_UTF32 = 27,
    PARSE_UTF32L = 28,
    PARSE_UTF32B = 29,
    PARSE_EXIT = 30,
    PARSE_ERROR = 31
};
typedef enum parse_mode parse_mode_t;

/* Returns true on little-endian system */
static int little_endian (void);


/* Convert string to character set */
charset_t
parse_charset (const char *p)
{
    unsigned i = 0;
    parse_mode_t mode = PARSE_INIT;
    int done = 0;
    charset_t result = INVALID_CHARSET;

    while (!done) {

        /* Get next character from string */
        char c = p[i];

        /* Parse character according to current mode */
        switch (mode) {
        case PARSE_INIT:
            /* Initial mode */
            switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                /* Ignore void space */
                i++;
                break;

            case 'a':
            case 'A':
                i++;
                mode = PARSE_A;
                break;

            case 'i':
            case 'I':
                i++;
                mode = PARSE_I;
                break;

            case 'l':
            case 'L':
                i++;
                mode = PARSE_L;
                break;

            case 'u':
            case 'U':
                i++;
                mode = PARSE_U;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_A:
            /* After A */
            switch (c) {
            case 's':
            case 'S':
                i++;
                mode = PARSE_AS;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_AS:
            /* After AS */
            switch (c) {
            case 'c':
            case 'C':
                i++;
                mode = PARSE_ASC;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ASC:
            /* After ASC */
            switch (c) {
            case 'i':
            case 'I':
                i++;
                mode = PARSE_ASCI;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ASCI:
            /* After ASCI */
            switch (c) {
            case 'i':
            case 'I':
                i++;
                mode = PARSE_EXIT;
                result = ASCII;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_I:
            /* After I */
            switch (c) {
            case 's':
            case 'S':
                i++;
                mode = PARSE_IS;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_IS:
            /* After IS */
            switch (c) {
            case 'o':
            case 'O':
                i++;
                mode = PARSE_ISO;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO:
            /* After ISO */
            switch (c) {
            case ' ':
            case '-':
                /* Ignore space or dash */
                i++;
                break;

            case '8':
                i++;
                mode = PARSE_ISO8;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO8:
            /* After ISO8 */
            switch (c) {
            case '8':
                i++;
                mode = PARSE_ISO88;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO88:
            /* After ISO88 */
            switch (c) {
            case '5':
                i++;
                mode = PARSE_ISO885;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO885:
            /* After ISO885 */
            switch (c) {
            case '9':
                i++;
                mode = PARSE_ISO8859;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO8859:
            /* After ISO8859 */
            switch (c) {
            case '-':
                i++;
                mode = PARSE_ISO8859DASH;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ISO8859DASH:
            /* After ISO8859 */
            switch (c) {
            case '1':
                i++;
                mode = PARSE_EXIT;
                result = ISO8859_1;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_L:
            /* After L */
            switch (c) {
            case 'a':
            case 'A':
                i++;
                mode = PARSE_LA;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_LA:
            /* After LA */
            switch (c) {
            case 't':
            case 'T':
                i++;
                mode = PARSE_LAT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_LAT:
            /* After LAT */
            switch (c) {
            case 'i':
            case 'I':
                i++;
                mode = PARSE_LATI;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_LATI:
            /* After LATI */
            switch (c) {
            case 'n':
            case 'N':
                i++;
                mode = PARSE_LATIN;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_LATIN:
            /* After LA */
            switch (c) {
            case ' ':
            case '-':
                /* Ignore space and dash */
                i++;
                break;

            case '1':
                i++;
                mode = PARSE_EXIT;
                result = ISO8859_1;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_U:
            /* After U */
            switch (c) {
            case 't':
            case 'T':
                i++;
                mode = PARSE_UT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UT:
            /* After UT */
            switch (c) {
            case 'f':
            case 'F':
                i++;
                mode = PARSE_UTF;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF:
            /* After UTF */
            switch (c) {
            case ' ':
            case '-':
                /* Ignore space or dash */
                i++;
                break;

            case '1':
                i++;
                mode = PARSE_UTF1;
                break;

            case '3':
                i++;
                mode = PARSE_UTF3;
                break;

            case '8':
                i++;
                mode = PARSE_EXIT;
                result = UTF8;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF1:
            /* After UTF1 */
            switch (c) {
            case '6':
                i++;
                mode = PARSE_UTF16;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF16:
            /* After UTF16 */
            switch (c) {
            case ' ':
                /* Ignore space */
                i++;
                break;

            case 'b':
            case 'B':
                i++;
                mode = PARSE_UTF16B;
                break;

            case 'l':
            case 'L':
                i++;
                mode = PARSE_UTF16L;
                break;

            default:
                /* Other character */
                result = UTF16;
                mode = PARSE_EXIT;
            }
            break;

        case PARSE_UTF16B:
            /* After UTF16B */
            switch (c) {
            case 'e':
            case 'E':
                i++;
                result = UTF16BE;
                mode = PARSE_EXIT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF16L:
            /* After UTF16L */
            switch (c) {
            case 'e':
            case 'E':
                i++;
                result = UTF16LE;
                mode = PARSE_EXIT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF3:
            /* After UTF3 */
            switch (c) {
            case '2':
                i++;
                mode = PARSE_UTF32;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF32:
            /* After UTF32 */
            switch (c) {
            case ' ':
                /* Ignore space */
                i++;
                break;

            case 'b':
            case 'B':
                i++;
                mode = PARSE_UTF32B;
                break;

            case 'l':
            case 'L':
                i++;
                mode = PARSE_UTF32L;
                break;

            default:
                /* Other character */
                result = UTF32;
                mode = PARSE_EXIT;
            }
            break;

        case PARSE_UTF32B:
            /* After UTF32B */
            switch (c) {
            case 'e':
            case 'E':
                i++;
                result = UTF32BE;
                mode = PARSE_EXIT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_UTF32L:
            /* After UTF32L */
            switch (c) {
            case 'e':
            case 'E':
                i++;
                result = UTF32LE;
                mode = PARSE_EXIT;
                break;

            default:
                /* Invalid character */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_EXIT:
            /* End of string */
            switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                /* Ignore void space after character set */
                i++;
                break;

            case '\0':
                /* End of string => character set successfully parsed */
                done = 1;
                break;

            default:
                /* Illegal characters after character set */
                mode = PARSE_ERROR;
            }
            break;

        case PARSE_ERROR:
            /* Error */
            done = 1;
            result = INVALID_CHARSET;
            break;

        default:
            /* Invalid mode */
            terminate ("Invalid mode");
        }

    }
    return result;
}


/* Convert character set to string */
const char *
nameof_charset (charset_t t)
{
    const char *name;

    switch (t) {
    case ASCII:
        name = "ascii";
        break;

    case ISO8859_1:
        name = "iso-8859-1";
        break;

    case FILESYSTEM_CHARSET:
        name = "fs";
        break;

    case LOCALE_CHARSET:
        name = "locale";
        break;

    case UTF8:
        name = "utf-8";
        break;

    case UTF16:
        name = "utf-16";
        break;

    case UTF16LE:
        name = "utf-16le";
        break;

    case UTF16BE:
        name = "utf-16be";
        break;

    case UTF32:
        name = "utf-32";
        break;

    case UTF32LE:
        name = "utf-32le";
        break;

    case UTF32BE:
        name = "utf-32be";
        break;

    case WCHAR:
        name = "wc";
        break;

    case INVALID_CHARSET:
    default:
        name = "invalid";
    }

    return name;
}


/* Convert pseudo character set to real character set */
charset_t
resolve_charset (charset_t t)
{
    charset_t r;

    switch (t) {
    case ASCII:
    case ISO8859_1:
    case UTF8:
    case UTF16LE:
    case UTF16BE:
    case UTF32LE:
    case UTF32BE:
        /* Not an alias */
        r = t;
        break;

    case FILESYSTEM_CHARSET:
        /* An operating system dependent character set */
        r = FILESYSTEM_CHARSET;
        break;

    case LOCALE_CHARSET:
        /* A user dependent character set */
        r = LOCALE_CHARSET;
        break;

    case UTF16:
        /* 16-bit character */
        if (little_endian ()) {
            r = UTF16LE;
        } else {
            r = UTF16BE;
        }
        break;

    case UTF32:
        /* 32-bit character */
        if (little_endian ()) {
            r = UTF32LE;
        } else {
            r = UTF32BE;
        }
        break;

    case WCHAR:
        /* Resolve character set of wchar recursively */
        assert (sizeof (wchar_t) == 2  ||  sizeof (wchar_t) == 4);
        if (sizeof (wchar_t) == 4) {

            /* 32-bit character (Linux/Unix) */
            r = resolve_charset (UTF32);

        } else {

            /* 16-bit character (Microsoft Windows) */
            r = resolve_charset (UTF16);

        }
        break;

    case INVALID_CHARSET:
    default:
        r = INVALID_CHARSET;
    }
    return r;
}


/* Returns true if running on little-endian system */
static int
little_endian (void)
{
    int r;

    union {
        short s;
        unsigned char c[2];
    } st;

    /* Assign 16-bit integer to union */
    st.s = 0x1234;
    assert (st.c[0] == 0x34  ||  st.c[0] == 0x12);

    /* Read back upper and lower byte */
    if (st.c[0] == 0x34) {

        /* Little-endian */
        r = 1;

    } else {

        /* Big endian */
        r = 0;

    }
    return r;
}


