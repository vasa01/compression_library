/**********************************************************************************************//**
* @file	cdf53.hpp
*
* @brief	Declares the cdf 5/3 class.
/**************************************************************************************************/

#ifndef CDF53_H
#define CDF53_H

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

#include "line.hpp"
#include "tool.hpp"
#include "dwtImage.hpp"
#include "dwtLevels.hpp"

/**********************************************************************************************//**
* @def	a1, a2, a3, a4, k1, k2
*
* @brief	A macros that defines coef on CDF9/7.
*
* @author	Vasa 0
* @date	06.01.2017
**************************************************************************************************/

#define k1 (1)
#define p1 (0.5)
#define u1 (0.25)

#define s1 1.41421356237309504880
#define s2 (0.70710678118654752440)

//static const int    dwt_cdf53_k_s = 1;
//static const float  dwt_cdf53_p1_s = 0.5;
//static const float  dwt_cdf53_u1_s = 0.25;
//static const float  dwt_cdf53_s1_s = 1.41421356237309504880;
//static const float  dwt_cdf53_s2_s = 0.70710678118654752440;


class CDF53 {
	int levels;
	int countChannels;
	cv::Mat image;
	cv::Mat forwardMat;
	cv::Mat inverseMat;
	vector<float> tempbank;
	DwtLevels *dwtLevels;
	int block_size;
public:

	CDF53(int levels, cv::Mat& image, int block_size=64);
	void forward2d(cv::Mat& image);
	void inverse2d(cv::Mat& image, int minlevel = 0);
	cv::Mat getForward() {
		return forwardMat;
		};
	cv::Mat getInverse() {
		return inverseMat;
		};
	double test();
	DwtLevels* getLevelsObject();
	//void getSubBands(vector<cv::Mat>& sub, cv::Mat& image);
	//double diff(const Mat& test, const Mat& ref);
	vector<DwtImage> CDF53::getLevels();
private:

	void inverse(Line<float> line);
	void inverse2(Line<float> line);
	void forward(Line<float> line);
	void predict(Line<float> line, double coef1, double coef2);
	void update(Line <float> line, double coef);
	};
#endif 

