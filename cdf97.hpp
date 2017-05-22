/**********************************************************************************************//**
 * @file	cdf97.hpp
 *
 * @brief	Declares the cdf 97 class.
 **************************************************************************************************/

#ifndef CDF97_H
#define CDF97_H

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

#define a1 (1.58613434342059)
#define a2 (-0.0529801185729)
#define a3 (-0.8829110755309)
#define a4 (0.4435068520439)

#define k1 1.1496043988602
#define k2 (1/1.1496043988602)

//using namespace cv;
//using namespace std;

/**********************************************************************************************//**
 * @class	CDF97
 *
 * @brief	A cdf 97.
 *
 * @author	Vasa 0
 * @date	06.01.2017
 **************************************************************************************************/

class CDF97 {
	int levels;
	int countChannels;
	cv::Mat image;
	cv::Mat forwardMat;
	cv::Mat inverseMat;
	vector<float> tempbank;
	DwtLevels *dwtLevels;
	int block_size;
	public:

		CDF97(int levels, cv::Mat& image, int block_size=64);
		void forward2d(cv::Mat& image, bool check = false);
		void inverse2d(cv::Mat& image, bool check = false);
		cv::Mat getForward() {
			return forwardMat;
		};
		cv::Mat getInverse() {
			return inverseMat;
		};
		double test();
		DwtLevels* getLevelsObject();
		void getSubBands(vector<cv::Mat>& sub, cv::Mat& image);
		//double diff(const Mat& test, const Mat& ref);
		vector<DwtImage> CDF97::getLevels();
	private:

		void inverse(Line<float> line);
		void inverse2(Line<float> line);
		void forward(Line<float> line);	
		void predict(Line<float> line, double coef1, double coef2);	
		void update(Line <float> line, double coef);
};
#endif 
