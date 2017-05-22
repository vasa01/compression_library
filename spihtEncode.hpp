#ifndef SPIHT_ENCODE_H
#define SPIHT_ENCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <deque>

#include "spiht.hpp"
#include "mycountourlet.hpp"

#include <opencv2/core/core.hpp>
#include "dwtLevels.hpp"
#include <memory>
#include <vector>

class SPIHT_Encoder_Contourlet {
	MyContourlet contourlet;
	LIP3 lip;
	LSP3 lsp;
	LIS3 lis;
	std::vector<std::vector<Matrix>> matrixHigh;
	int maxChild;
	Matrix matrixLow;
	int step;    /* quantization step */
	void get_successor(int x, int y, int* sx, int* sy, int childNumber, int direction, SPIHT_Type s_type);
	bool is_significant_pixel(int x, int y, int childNumber,int direction, SPIHT_Type s_type);
	bool is_significant_set_A(int x, int y, int childrenNumber,int direction , SPIHT_Type s_type, int count = 1);
	bool is_significant_set_B(int x, int y, int childrenNumber, int direction, SPIHT_Type s_type, int count = 1);
	void initialize(BitOutputStream* bout);
	int32_t get(cv::Point, int childNumber, int direction, SPIHT_Type s_type);
public:
	SPIHT_Encoder_Contourlet(MyContourlet _contourlet, int _num_stages) {
		contourlet = _contourlet;

	}
	void encode(int bits, BitOutputStream* bout);
};

class SPIHT_Encoder {
	Matrix* im;
	LIP lip;
	LSP lsp;
	LIS lis;
	DwtLevels* levels;
	int step;    /* quantization step */
	void get_successor(int x, int y, int* sx, int* sy, int childNumber, SPIHT_Type s_type);
	bool is_significant_pixel(int x, int y, int childNumber, SPIHT_Type s_type);
	bool is_significant_set_A(int x, int y, int childrenNumber, SPIHT_Type s_type, int count = 1);
	bool is_significant_set_B(int x, int y, int childrenNumber, SPIHT_Type s_type, int count=1);
	void initialize( BitOutputStream* bout);
	vector<Matrix> subIm;
	SPIHT spiht;
public:
	SPIHT_Encoder(){};
	SPIHT_Encoder(Matrix* _im, DwtLevels* _level, int _num_stages, int channel) { 
		im = _im; 
		//num_stages = _num_stages - 2;  
		levels = _level; 
	
		vector<DwtImage*> temp = levels->getImagesVec();
		Matrix LL;
		vector<Matrix> HL;
		vector<Matrix> HH;
		vector<Matrix> LH;
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i]->getTypeSubBands() == Bands::LL && temp[i]->getChannel()==channel) {
				LL = Matrix(temp[i]->getImage(),1);
			}
			else if (temp[i]->getTypeSubBands() == Bands::HL && temp[i]->getChannel() == channel) {
				HL.push_back(Matrix(temp[i]->getImage(), 1));
			}
			else if (temp[i]->getTypeSubBands() == Bands::HH && temp[i]->getChannel() == channel) {
				HH.push_back(Matrix(temp[i]->getImage(), 1));
			}
			else if (temp[i]->getTypeSubBands() == Bands::LH && temp[i]->getChannel() == channel) {
				LH.push_back(Matrix(temp[i]->getImage(), 1));
			}
			//subIm.push_back(Matrix(temp[i]->getImage()));
			//cout << subIm[i].h() << endl;
		}
		spiht = SPIHT(LL, HL, HH, LH);

		//cv::Mat LLmat, tmp2;
		//LLmat = im->getCvMatDequant(100);//LL.getCvMatDequant(1000);
		//LLmat.convertTo(tmp2, CV_8UC1);
		//cv::imshow("LL_puvod", tmp2);
		//cv::waitKey();
		//cout << "sem"<< temp.size() << endl;

	}
	void encode( int bits, BitOutputStream* bout);
};

#endif 