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
 * @file	ari.hpp
 *
 * @brief	arithmetic.
 */

#ifndef ARI_H
#define ARI_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <climits>

static const uint32_t MAX_FREQ = (1U << 29) - 1U;
/**
 * TypeWithSize
 * @tparam N
 */
template <size_t N>
struct TypeWithSize
	{
	typedef void Uint;
	typedef void Int;
	};
/**
 * TypeWithSize 4
 */
template <>
struct TypeWithSize<4>
	{
	typedef uint32_t Uint;
	typedef int32_t Int;
	};
/**
 * TypeWithSize 8
 */
template <>
struct TypeWithSize<8>
	{
	typedef uint64_t Uint;
	typedef int64_t Int;
	};

/**
 * class IntervalTraits
 * @tparam N
 */
template< size_t N >
class IntervalTraits {
public:
	//typedef typename TypeWithSize<N>::Uint ValueType;

	//std::cout << N << std::endl;

	/// Number of bits we use. -1 is because without it overflow could occur
	static const size_t BITS = N * 8 - 1;
	/// Max interval value
	static const uint32_t MAX = (1U << BITS) - 1U;
	/// Quarter of maximum interval value
	static const uint32_t QUARTER = MAX / 4U + 1;
	/// Half of maximum interval value
	static const uint32_t HALF = 2 * QUARTER;
	/// Three quarters of maximum interval value
	static const uint32_t THREE_QUARTERS = 3 * QUARTER;
};

#endif
