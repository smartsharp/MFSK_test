// ----------------------------------------------------------------------------
// util.cxx
//
// Copyright (C) 2007-2009
//		Stelios Bounanos, M0GLD
// Copyright (C) 2009
//		Dave Freese, W1HKJ
// Copyright (C) 2013
//		Remi Chateauneu, F4ECW
// Copyright (C) 2015
//		Robert Stiles, KK5VD
//
// This file is part of fldigi.
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <cstdlib>
#include <string>

//#include "config.h"
#include "util.h"


/** ********************************************************************
 * Return the smallest power of 2 not less than n
 ***********************************************************************/
uint32_t ceil2(uint32_t n)
{
        --n;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        return n + 1;
}

/** ********************************************************************
 * Return the largest power of 2 not greater than n
 ***********************************************************************/
/// Return the largest power of 2 not greater than n
uint32_t floor2(uint32_t n)
{
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;

        return n - (n >> 1);
}

#include <stdlib.h>

/** ********************************************************************
 * Transforms the version, as a string, into an integer, so comparisons
 * are possible.
 ***********************************************************************/
unsigned long ver2int(const char* version)
{
	unsigned long v = 0L;
	const char* p = version;
	while (*p) {
		if (isdigit(*p)) v = v*10 + *p - '0';
		p++;
	}

	return v;
}


/** ********************************************************************
 *
 ***********************************************************************/
uint32_t simple_hash_data(const unsigned char* buf, size_t len, uint32_t code)
{
	for (size_t i = 0; i < len; i++)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)buf[i];

	return code;
}

/** ********************************************************************
 *
 ***********************************************************************/
uint32_t simple_hash_str(const unsigned char* str, uint32_t code)
{
	while (*str)
		code = ((code << 4) | (code >> (32 - 4))) ^ (uint32_t)*str++;
	return code;
}

#include <vector>
#include <climits>

static const char hexsym[] = "0123456789ABCDEF";

static std::vector<char>* hexbuf;
/** ********************************************************************
 *
 ***********************************************************************/
const char* str2hex(const unsigned char* str, size_t len)
{
	if (unlikely(len == 0))
		return "";
	if (unlikely(!hexbuf)) {
		hexbuf = new std::vector<char>;
		hexbuf->reserve(192);
	}
	if (unlikely(hexbuf->size() < len * 3))
		hexbuf->resize(len * 3);

	char* p = &(*hexbuf)[0];
	size_t i;
	for (i = 0; i < len; i++) {
		*p++ = hexsym[str[i] >> 4];
		*p++ = hexsym[str[i] & 0xF];
		*p++ = ' ';
	}
	*(p - 1) = '\0';

	return &(*hexbuf)[0];
}

/** ********************************************************************
 *
 ***********************************************************************/
const char* str2hex(const char* str, size_t len)
{
	return str2hex((const unsigned char*)str, len ? len : strlen(str));
}

static std::vector<char>* binbuf;
/** ********************************************************************
 *
 ***********************************************************************/
const char* uint2bin(unsigned u, size_t len)
{
	if (unlikely(len == 0))
		len = sizeof(u) * CHAR_BIT;

	if (unlikely(!binbuf)) {
		binbuf = new std::vector<char>;
		binbuf->reserve(sizeof(u) * CHAR_BIT);
	}
	if (unlikely(binbuf->size() < len + 1))
		binbuf->resize(len + 1);

	for (size_t i = 0; i < len; i++) {
		(*binbuf)[len - i - 1] = '0' + (u & 1);
		u >>= 1;
	}
	(*binbuf)[len] = '\0';

	return &(*binbuf)[0];
}

/** ********************************************************************
 *
 ***********************************************************************/
void MilliSleep(long msecs)
{
#ifndef __MINGW32__
	struct timespec tv[2] = { {msecs / 1000L, msecs % 1000L * 1000000L} };
	nanosleep(&tv[0], &tv[1]);
#else
	Sleep(msecs);
#endif
}


