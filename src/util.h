/* -----------------------------------------------------------------------------
 *    util.h -- included by config.h
 *
 *    Copyright (C) 2007-2009
 *   		Stelios Bounanos, M0GLD
 *
 *    This file is part of fldigi.
 *
 *    Fldigi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Fldigi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 */

#ifndef UTIL_H
#define UTIL_H

// include the system defines
//#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#ifndef powerof2
#    define powerof2(n) ((((n) - 1) & (n)) == 0)
#endif
#ifndef MAX
#    define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#    define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef CLAMP
#    define CLAMP(x, low, high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))
#endif
#define WCLAMP(x, low, high) (((x)>(high))?(low):(((x)<(low))?(high):(x)))

/* http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html */
#if defined(__GNUC__) && (__GNUC__ >= 3)
#    define likely(x)    __builtin_expect (!!(x), 1)
#    define unlikely(x)  __builtin_expect (!!(x), 0)
#    define used__       __attribute__ ((__used__))
#    define unused__     __attribute__ ((__unused__))
#    define must_check__ __attribute__ ((__warn_unused_result__))
#    define deprecated__ __attribute__ ((__deprecated__))
#    define noreturn__   __attribute__ ((__noreturn__))
#    define pure__       __attribute__ ((__pure__))
#    define const__      __attribute__ ((__const__))
#    define malloc__     __attribute__ ((__malloc__))
#    define packed__     __attribute__ ((__packed__))
#    define inline__     inline __attribute__ ((__always_inline__))
#    define noinline__   __attribute__ ((__noinline__))
#    define nonnull__(x) __attribute__ ((__nonnull__(x)))
#    define format__(type_, index_, first_) __attribute__ ((format(type_, index_, first_)))
#else
#    define likely(x)    (x)
#    define unlikely(x)  (x)
#    define used__
#    define unused__
#    define must_check__
#    define deprecated__
#    define noreturn__
#    define pure__
#    define const__
#    define malloc__
#    define packed__
#    define inline__
#    define noinline__
#    define nonnull__(x)
#    define format__(type_, index_, first_)
#endif

#include <stddef.h>

uint32_t ceil2(uint32_t n);
uint32_t floor2(uint32_t n);

void MilliSleep(long msecs);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
uint32_t simple_hash_data(const unsigned char* buf, size_t len, uint32_t code = 0);
uint32_t simple_hash_str(const unsigned char* str, uint32_t code = 0);
#endif

#ifdef __cplusplus
const char* str2hex(const unsigned char* str, size_t len);
const char* str2hex(const char* str, size_t len = 0);
#else
const char* str2hex(const unsigned* str, size_t len);
#endif

const char* uint2bin(unsigned u, size_t len);

#endif /* UTIL_H */

/*
Local Variables:
mode: c++
c-file-style: "linux"
End:
*/
