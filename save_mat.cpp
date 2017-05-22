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
 * @file	save_mat.cpp
 *
 * @brief	Save mat to cv::Mat.
 */


//#include"stdafx.h"
//#include "opencv/cv.h"
//#include "opencv/highgui.h"
//#include <time.h>
//#include "io.hpp"
//#include "distance.hpp"
//#include <stdio.h>
//#include <math.h>
////#include "it/wavelet2D.h"
//#include "mat.hpp"
//#include "contourlet.hpp"
//#include "dfb.hpp"
////#include "ezbc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include "save_mat.hpp"
/**
 * save_mat
 * @param m
 * @return - IplImage*
 */
IplImage* save_mat(mat m)
{ 
	int i,j;
	int width=mat_height(m);
	int height=mat_width(m);


	IplImage* img = cvCreateImage(cvSize(height,width),8,1);
	std::cout << "sem " << img->height << " " << img->width << std::endl;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
			{
				cvSetReal2D(img, i,j,m[i][j] );
				
			}
	}
	std::cout << "tes" << std::endl;
		return img;
};

/**
 * save_mat
 * @param m
 * @return cv::Mat
 */
cv::Mat saveMat(mat m)
{
	int i, j;
	int width = mat_width(m);
	int height = mat_height(m);
	cv::Mat tmpMat = cv::Mat(height, width, CV_32FC1);
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++) {
			tmpMat.at<float>(i, j) = m[i][j];
		}
	}
	return tmpMat;
};