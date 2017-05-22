/**
 * Image compression library supporting wavelet and contourlet
 * transformation with the possibility of encoding algorithms EZW, SPIHT and EBCOT.
 * (C) Vaclav Bradac
 *
 * This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/**
 * @file	iOStream.cpp
 *
 * @brief	iOStream implementation.
 */

#ifndef IOSTREAM_H
#define IOSTREAM_H

#include <stddef.h>

#ifndef EOF
#define EOF (-1)
#endif

/**
 * class InputStream
 */
class InputStream {
public:
	virtual int get_char() = 0;
	virtual long get_size() = 0;   /* get stream total size */
	};

/**
 * class OutputStream
 */
class OutputStream {
public:
	virtual void put_char(int c) = 0;
	virtual void flush() = 0;      /* flush the stream */
	};

#endif
