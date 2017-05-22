#ifndef ARI_H
#define ARI_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <climits>

static const uint32_t MAX_FREQ = (1U << 29) - 1U;

template <size_t N>
struct TypeWithSize
	{
	typedef void Uint;
	typedef void Int;
	};

template <>
struct TypeWithSize<4>
	{
	typedef uint32_t Uint;
	typedef int32_t Int;
	};

template <>
struct TypeWithSize<8>
	{
	typedef uint64_t Uint;
	typedef int64_t Int;
	};


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