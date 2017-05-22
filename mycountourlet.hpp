#ifndef MYCONTOURLET_H
#define MYCONTOURLET_H

#include "opencv\cv.h"
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

class MyContourlet {
public:
	MyContourlet(contourlet_t *contourlet);
	MyContourlet(std::vector<std::vector<Matrix>> _high, Matrix _low, int ct_levels);
	MyContourlet(std::vector<std::vector<cv::Mat>> _high, cv::Mat _low, int ct_levels);
	MyContourlet() {
	}
	vector<DwtImage*> getBlocks(int block_size);
	contourlet_t *getStruct();
	cv::Mat getLow() {
		return low;
	}
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

