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
 * @file	mycontourlet.hpp
 *
 * @brief	Mycontourlet implementation.
 */


#ifndef MYCONTOURLET_H
#define MYCONTOURLET_H

//#include "opencv\cv.h"
#include "opencv2/opencv.hpp"
#include "mat.hpp"
#include "contourlet.hpp"
#include "load_mat.hpp"
#include "save_mat.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include"matrix.hpp"
#include "dwtImage.hpp"
#include "tool.hpp"


/**
 * class MyContourlet
 */
class MyContourlet {
public:
	MyContourlet(contourlet_t *contourlet);
	MyContourlet(std::vector<std::vector<Matrix>> _high, Matrix _low, int ct_levels);
	MyContourlet(std::vector<std::vector<cv::Mat>> _high, cv::Mat _low, int ct_levels);
	MyContourlet() {
	}
	vector<DwtImage*> getBlocks(int block_size);
	contourlet_t *getStruct();
	/**
	 * getLow
	 * @return  - low cv::Mat
	 */
	cv::Mat getLow() {
		return low;
	}

	/**
	 * getHigh()
	 * @return - high vector<cv::Mat
	 */
	std::vector<std::vector<cv::Mat>> getHigh() {
		return high;
	}
private:
	int ct_levels;   /* number of pyramid decomposition levels     */
	int wt_levels;   /* number of wavelet decomposition levels     */
	std::vector<int> dfb_levels; /* number of directional decomposition levels */
	cv::Mat low;         /* low-frequency band                         */
	std::vector<std::vector<cv::Mat>> high;      /* high-frequency bands [level][orientation]  */
	//std::vector<cv::Mat> dwt;        /* wavelet subband decomposition of 'low' */
	std::vector<double> H0;          /* pyramid analysis filter */
	std::vector<double> G0;
	
};

#endif

