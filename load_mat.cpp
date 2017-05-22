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
 * @file	load_mat.cpp
 *
 * @brief	load mat implementation.
 */

//#include "stdafx.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <time.h>
#include "io.hpp"
#include "distance.hpp"
#include <stdio.h>
#include <math.h>
//#include "it/wavelet2D.h"
#include "mat.hpp"
#include "contourlet.hpp"
#include "dfb.hpp"
//#include "ezbc.h"
#include "load_mat.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/**
 * load mat from IplImage img
 * @param img
 * @return - mat
 */
mat load_mat(IplImage* img)

{

       int i,j;

       int width=img->width;

       int height=img->height;  

       mat m = mat_new (height, width);

       for(i=0;i<height;i++)

       {

              for(j=0;j<width;j++)

              {

                     m[i][j]=cvGetReal2D(img, i, j);

              }

       }

       return m;

};

/**
 * load mat from cv::Mat img
 * @param img
 * @return - mat
 */
mat loadMat(cv::Mat img)

{

	int i, j;
	img.convertTo(img, CV_32F);

	int width = img.cols;

	int height = img.rows;

	mat m = mat_new(height, width);
	//std::cout << "width: "<<width<< " height: "<< height << std::endl;
	for (i = 0; i<height; i++)

	{

		for (j = 0; j<width; j++)

		{
			m[i][j] = (double)img.at<float>(i, j);//img.at<float>(i,j);

		}

	}
	//std::cout << "sem" << std::endl;
	return m;

};