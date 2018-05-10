/*
 * Test-Driven Development Framework 7 for C
 *
 * Copyright (C) 2018 Toni Ronkko
 * This file is part of T7.  T7 may be freely distributed under the MIT
 * license.  For more information, see https://github.com/tronkko/t7
 */
#ifndef T7_CHARSET_H
#define T7_CHARSET_H
#ifdef __cplusplus
extern "C" {
#endif


/****s* libt7/charset_enum
 * NAME
 * charset_enum - supported character sets
 *
 * FUNCTION
 * List of supported character sets.
 *
 * SOURCE
 */
enum charset_enum {
    INVALID_CHARSET = 0,
    UTF8 = 1,
    ISO8859_1 = 2,
    ASCII = 3,
    FILESYSTEM_CHARSET = 4,
    LOCALE_CHARSET = 5,
    UTF16 = 6,
    UTF16LE = 7,
    UTF16BE = 8,
    UTF32 = 9,
    UTF32LE = 10,
    UTF32BE = 11,
    WCHAR = 12
};
/****/


/****t* libt7/charset_t
 * NAME
 * charset_t - character set
 *
 * FUNCTION
 * An enumeration containing one of the supported character sets from the
 * table below:
 *
 *     Enumeration        | Character Set
 *     -------------------+---------------------------------------------------
 *     INVALID_CHARSET    | Unknown or invalid character set
 *     UTF8               | UTF-8 with at most 4-byte code paths
 *     ISO8859_1          | ISO 8859-1
 *     ASCII              | Plain ASCII
 *     FILESYSTEM_CHARSET | As defined by the file system
 *     LOCALE_CHARSET     | As defined by the current locale
 *     UTF16              | UTF-16 pseudo character set
 *     UTF16LE            | UTF-16 little endian
 *     UTF16BE            | UTF-16 big endian
 *     UTF32              | UTF-32 pseudo character set
 *     UTF32LE            | UTF-32 little endian
 *     UTF32BE            | UTF-32 big endian
 *     WCHAR              | As defined by the wchar_t data type
 *
 * See charset_enum for list of character sets and the associated numeric
 * values.
 *
 * SOURCE
 */
typedef enum charset_enum charset_t;
/****/


/****f* libt7/parse_charset
 * NAME
 * parse_charset - convert character set to enumeration
 *
 * FUNCTION
 * Parse character set contained in the string P and return corresponding
 * charset_t.  If the string does not contain a valid character set, then the
 * function returns INVALID_CHARSET.
 *
 * EXAMPLE
 * // Returns UTF8
 * charset_t ch = parse_charset ("utf8");
 *
 * SYNOPSIS
 */
charset_t parse_charset (const char *p);
/****/


/****f* libt7/nameof_charset
 * NAME
 * nameof_charset - convert enumeration to character set
 *
 * FUNCTION
 * Get pointer to zero-terminated character string which corresponds to the
 * character set enumeration T.  If the character set is invalid, then the
 * function returns the string "invalid".
 *
 * EXAMPLE
 * // Returns "utf-8"
 * const char *p = nameof_charset (UTF8);
 *
 * SYNOPSIS
 */
const char *nameof_charset (charset_t t);
/****/


/****f* libt7/resolve_charset
 * NAME
 * resolve_charset - resolve character set aliases
 *
 * FUNCTION
 * Convert pseudo character set T into machine dependent character set such as
 * UTF16LE (lower-endian) or UTF16BE (big-endian).
 *
 * EXAMPLE
 * // Returns UTF16LE or UTF16BE depending on the platform
 * charset_t t = resolve_charset (UTF16);
 *
 * // Returns UTF16LE, UTF16BE, UTF32LE or UTF32BE depending on the platform
 * charset_t t = resolve_charset (WCHAR);
 *
 * SYNOPSIS
 */
charset_t resolve_charset (charset_t t);
/****/


#ifdef __cplusplus
}
#endif
#endif /*T7_CHARSET_H*/

