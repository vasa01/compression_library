#ifndef SPIHT_DECODE_H
#define SPIHT_DECODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include "joinSubimage.hpp"
#include "mycountourlet.hpp"
#include "spiht.hpp"
#include "tool.hpp"

//using namespace std;
//SPIHT_Encoder_Contourlet
class SPIHT_Decoder_Contourlet {
	//Matrix *im;
	int num_stages;
	int level;
	LIP3 lip;
	LSP3 lsp;
	LIS3 lis;
	int width;
	int height;
	int step;    /* quantization step */
	int maxChild;
	std::vector<std::vector<Matrix>> matrixHigh;
	Matrix matrixLow;
	//SPIHT spiht;
	void get_successor(int x, int y, int* sx, int* sy, int childNumber, int direction, SPIHT_Type s_type);
	void initialize(int component, BitInputStream* bin);
	void set(cv::Point point, int childNumber, int direction, SPIHT_Type s_type, int32_t value);
	int32_t get(cv::Point point, int childNumber, int direction, SPIHT_Type s_type);
public:
	SPIHT_Decoder_Contourlet() {
		


	}
	void decode(int component, int bits, BitInputStream* bin);

	MyContourlet getOutput();

};

class SPIHT_Decoder {
	//Matrix *im;
	int num_stages;
	int level;
	LIP lip;
	LSP lsp;
	LIS lis;
	int width;
	int height;
	int step;    /* quantization step */
	SPIHT spiht;
	void get_successor(int x, int y, int* sx, int* sy, int childNumber, SPIHT_Type s_type);
	void initialize(int component, BitInputStream* bin);
public:
	SPIHT_Decoder(Matrix* _im, int _level, int _width, int _height) { 
		//im = _im; 
		level = _level;  
		width = _width;
		height = _height;

		Matrix LL;
		vector<Matrix> HL;
		vector<Matrix> HH;
		vector<Matrix> LH;
		int LLWidth, LLHeight;
		LLWidth = width / Tool::GetPower(2, level);
		LLHeight = height / Tool::GetPower(2, level);
		LL = Matrix(LLWidth,LLHeight);
		for (int i = 0; i < level; i++) {
			HL.push_back(Matrix(LLWidth, LLHeight));
			HH.push_back(Matrix(LLWidth, LLHeight));
			LH.push_back(Matrix(LLWidth, LLHeight));

			LLWidth *= 2;
			LLHeight *= 2;
		}
		spiht = SPIHT(LL, HL, HH, LH);
		

	}
	void decode(int component, int bits, BitInputStream* bin);
	
	Matrix getOutput(int block_size);

};



#endif
