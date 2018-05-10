/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#include "t7/types.h"
#include "t7/charset.h"

#undef NDEBUG
#include <assert.h>


int
main (void)
{
    charset_t t;

    /* Empty string is not a valid character set */
    t = parse_charset ("");
    assert (t == INVALID_CHARSET);

    /* Invalid character string always produces INVALID_CHARSET */
    t = parse_charset ("Bad puppy!");
    assert (!t);
    assert (t == INVALID_CHARSET);

    /* Convert ASCII to enum */
    t = parse_charset ("ascii");
    assert (t == ASCII);

    /* Character set may be given in capitals */
    t = parse_charset ("ASCII");
    assert (t == ASCII);

    /* Void space before character set is tolerated */
    t = parse_charset ("  \n\t\r ascii");
    assert (t == ASCII);

    /* Void space after character set is tolerated */
    t = parse_charset ("  \n\t\r ascii \r\n\t ");
    assert (t == ASCII);

    /* Excess letters after character set name render the name invalid */
    t = parse_charset ("  \n\t\r ascii \r\n\t x");
    assert (t == INVALID_CHARSET);

    /* UTF-8 is parsed properly without dash or space */
    t = parse_charset ("utf8");
    assert (t == UTF8);

    /* UTF-8 is parsed properly with space */
    t = parse_charset ("utf 8");
    assert (t == UTF8);

    /* UTF-8 is parsed properly with dash */
    t = parse_charset ("utf-8");
    assert (t == UTF8);

    /* UTF-8 is parsed with capital letters and excess space */
    t = parse_charset ("  UTF-8 ");
    assert (t == UTF8);

    /* Parse UTF-16, UTF-16LE and UTF16BE */
    t = parse_charset ("utf16");
    assert (t == UTF16);
    t = parse_charset ("utf-16be");
    assert (t == UTF16BE);
    t = parse_charset ("utf 16BE");
    assert (t == UTF16BE);
    t = parse_charset ("utf-16le");
    assert (t == UTF16LE);

    /* Parse UTF-32, UTF-32LE and UTF32BE */
    t = parse_charset ("utf32");
    assert (t == UTF32);
    t = parse_charset ("UTF-32BE");
    assert (t == UTF32BE);
    t = parse_charset ("utf32be");
    assert (t == UTF32BE);
    t = parse_charset ("Utf32le");
    assert (t == UTF32LE);
    t = parse_charset ("Utf32LE");
    assert (t == UTF32LE);

    /* Parse ISO-8859-1 */
    t = parse_charset ("ISO8859-1");
    assert (t == ISO8859_1);
    t = parse_charset ("iso-8859-1");
    assert (t == ISO8859_1);

    /* ISO-8859-1111 is not a valid character set */
    t = parse_charset ("iso-8859-1111");
    assert (!t);

    /* Latin1 is an alias for ISO-8859-1 */
    t = parse_charset ("Latin1");
    assert (t == ISO8859_1);
    t = parse_charset ("LATIN 1");
    assert (t == ISO8859_1);
    t = parse_charset ("latin-1");
    assert (t == ISO8859_1);

    /* UTF16 may only be mapped to UTF16LE or UTF16BE */
    t = resolve_charset (UTF16);
    assert (t == UTF16LE  ||  t == UTF16BE);

    /* UTF32 may only be mapped to UTF32LE or UTF32BE */
    t = resolve_charset (UTF32);
    assert (t == UTF32LE  ||  t == UTF32BE);

    /* WCHAR may be mapped to UTF16LE, UTF16BE, UTF32LE or UTF32BE */
    t = resolve_charset (WCHAR);
    assert (t == UTF16LE || t == UTF16BE || t == UTF32LE || t == UTF32BE);

    return 0;
}

