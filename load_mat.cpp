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