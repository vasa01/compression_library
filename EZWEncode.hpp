#pragma once
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



class EZWEncode {
public:
	EZWEncode();
    void EZWEncode::encode(Matrix& mat, int32_t minThreshold = 0);
	BitOutputStream* EZWEncode::getBitstream();
	ostringstream* EZWEncode::getBitstreamString();
	deque<uint8_t> EZWEncode::getSymbols();
	int32_t EZWEncode::getThreshold();
private:
	deque<Element> elements;
	list<int> subordList;
	BitOutputStream* bitStreamWriter;
	ostringstream temp;
	deque<uint8_t> symbols;
	int32_t defaultThreshold;
    int32_t EZWEncode::computeThreshold(Matrix mat);
    void EZWEncode::init(Matrix& mat, int32_t threshold);
    Element EZWEncode::codeElm(Matrix& mat, int x, int y, int32_t threshold);
    Element::Code EZWEncode::computeElmCode(Matrix& mat, int x, int y, int32_t threshold);
    bool EZWEncode::isZerotreeRoot(Matrix& mat, int x, int y, int32_t threshold);
    void EZWEncode::dominantPass(Matrix& mat, int32_t threshold);
	void EZWEncode::subordinatePass(int32_t threshold, int32_t minThreshold);
	void EZWEncode::outputCode(Element::Code code);
};



#endif