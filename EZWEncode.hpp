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
 * @file	EZWEncode.hpp
 *
 * @brief	EZWEncode implementation.
 */


#ifndef EZWENCODE_H
#define EZWENCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <deque>
#include <list>
#include "fileIOStream.hpp"
#include "tool.hpp"
#include "bitIOStream.hpp"


/**
 * class EZWEncode
 */
class EZWEncode {
public:
	EZWEncode();
    void encode(Matrix mat, int32_t minThreshold = 0);
	BitOutputStream* getBitstream();
	ostringstream* getBitstreamString();
	deque<uint8_t> getSymbols();
	int32_t getThreshold();
private:
	deque<Element> elements;
	list<int> subordList;
	BitOutputStream* bitStreamWriter;
	ostringstream temp;
	deque<uint8_t> symbols;
	int32_t defaultThreshold;
    int32_t computeThreshold(Matrix mat);
    void init(Matrix& mat, int32_t threshold);
    Element codeElm(Matrix& mat, int x, int y, int32_t threshold);
    Element::Code computeElmCode(Matrix& mat, int x, int y, int32_t threshold);
    bool isZerotreeRoot(Matrix& mat, int x, int y, int32_t threshold);
    void dominantPass(Matrix& mat, int32_t threshold);
	void subordinatePass(int32_t threshold, int32_t minThreshold);
	void outputCode(Element::Code code);
};



#endif
