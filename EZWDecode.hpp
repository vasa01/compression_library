#pragma once
#ifndef EZWDECODE_H
#define EZWDECODE_H

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
#include "bitIOStream.hpp"

#include "tool.hpp"



struct ElementCoord {
	ElementCoord() { }
	ElementCoord(int x, int y): x(x), y(y) { }
	size_t x, y;
};


class EZWDecode {
public:
	EZWDecode(deque<unsigned> symbols, BitInputStream* bitStreamReader);
	void EZWDecode::decode(int32_t threshold, int32_t minThreshold, Matrix& mat);
private:
	void EZWDecode::dominantPass(int32_t threshold, Matrix& mat);
	void EZWDecode::subordinatePass(int32_t threshold, int32_t minThreshold, Matrix& mat);
	void EZWDecode::init(int32_t threshold, Matrix& m);
	Element EZWDecode::decodeElement(int32_t threshold, int x, int y, Matrix& m);
	Element::Code EZWDecode::readElementCode();
	int pixels;
	vector<ElementCoord> subordVec;
	deque<unsigned> symbols;
	BitInputStream* bitStreamReader;
	deque<Element> elements;
};

#endif