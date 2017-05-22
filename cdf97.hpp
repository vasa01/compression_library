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
 * @file	cdf97.hpp
 *
 * @brief	Implements the cdf 97 class.
 */

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


const double a1 =  (1.58613434342059);
const double a2 =  (-0.0529801185729);
const double a3 =  (-0.8829110755309);
const double a4 =  (0.4435068520439);

const double k1_0 =  1.1496043988602;
const double k2_0 =  (1/1.1496043988602);


/**
 * class CDF97
 */
class CDF97 {
	int levels;
	int countChannels;
	cv::Mat image;
	cv::Mat forwardMat;
	cv::Mat inverseMat;
	vector<float> tempbank;
	DwtLevels *dwtLevels;
	int block_size;
	int quant;
	public:

		CDF97(int levels, cv::Mat& image, int quant, int block_size=64);
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
		vector<DwtImage> getLevels();
	private:

		void inverse(Line<float> line);
		void inverse2(Line<float> line);
		void forward(Line<float> line);	
		void predict(Line<float> line, double coef1, double coef2);	
		void update(Line <float> line, double coef);
};
#endif 
