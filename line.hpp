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
 * @file	line.cpp
 *
 * @brief	Line implementation.
 */

#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

/**
 * class Line
 * @tparam T
 */
template <typename T>
class Line
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef size_t size_type;


	/**
	 * create line obj from pointer to len
	 * @param ptr - pointer
	 * @param n - len
	 */
	Line(pointer ptr, size_type n) : ptr(ptr), n(n) { }

	/**
	 * create Line from vector
	 * @param vec
	 */
	Line(std::vector<T> &vec) : ptr(vec.data()), n(vec.size()) { }


	/**
	 * size
	 * @return size line
	 */
	size_type size() const {
		return n;
	}

	/**
	 * operator []
	 * @param i
	 * @return values type
	 */
	value_type& operator[](size_type i) {
		assert(i < n);
		return ptr[i];
	}


	/**
	 *
	 * @param i
	 * @return value
	 */
	const value_type& operator[](size_type i) const {
		assert(i < n);
		return ptr[i];
	}
private:
	T* ptr;
	size_type n;
};
#endif
