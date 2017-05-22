/**********************************************************************************************//**
 * @file	line.hpp
 *
 * @brief	Declares the line class.
 **************************************************************************************************/

#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>

//using namespace std;

/**********************************************************************************************//**
 * @class	Line
 *
 * @brief	A line.
 *
 * @author	Vasa 0
 * @date	06.01.2017
 *
 * @tparam	T	Generic type parameter.
 **************************************************************************************************/

template <typename T>
class Line
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef size_t size_type;

	/**********************************************************************************************//**
	 * @fn	Line::Line(pointer ptr, size_type n)
	 *
	 * @brief	Constructor.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	ptr	The pointer.
	 * @param	n  	The size_type to process.
	 **************************************************************************************************/

	Line(pointer ptr, size_type n) : ptr(ptr), n(n) { }

	/**********************************************************************************************//**
	 * @fn	Line::Line(vector<T>& vec)
	 *
	 * @brief	Constructor.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	vec	The vector.
	 **************************************************************************************************/

	Line(std::vector<T>& vec) : ptr(vec.data()), n(vec.size()) { }

	/*pointer data() {
		return ptr;
	}

	const pointer data() const {
		return ptr;
	}
	*/

	/**********************************************************************************************//**
	 * @fn	size_type size() const
	 *
	 * @brief	Gets the size.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	A size_type.
	 **************************************************************************************************/

	size_type size() const {
		return n;
	}

	/**********************************************************************************************//**
	 * @fn	value_type& operator[](size_type i)
	 *
	 * @brief	Array indexer operator.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	i	Zero-based index of the.
	 *
	 * @return	The indexed value.
	 **************************************************************************************************/

	value_type& operator[](size_type i) {
		assert(i < n);
		return ptr[i];
	}

	/**********************************************************************************************//**
	 * @fn	const value_type& operator[](size_type i) const
	 *
	 * @brief	Array indexer operator.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	i	Zero-based index of the.
	 *
	 * @return	The indexed value.
	 **************************************************************************************************/

	const value_type& operator[](size_type i) const {
		assert(i < n);
		return ptr[i];
	}
private:
	T* ptr;
	size_type n;
};
#endif