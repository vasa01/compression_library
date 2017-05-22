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
IplImage* save_mat(mat m)
{ 
	int i,j;
	int width=mat_height(m); //矩阵列数为图像宽
	int height=mat_width(m); //矩阵行数为图像高

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