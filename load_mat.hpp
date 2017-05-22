
#ifndef LOAD_MAT_H
#define LOAD_MAT_H

#include "opencv/cv.h"

//#include "opencv\cv.hpp"
//#include "opencv/highgui.h"
//#include <time.h>
//#include "io.hpp"
//#include "distance.hpp"
//#include <stdio.h>
//#include <math.h>
//#include "wavelet2D.hpp"
#include "mat.hpp"
//#include "contourlet.hpp"
//#include "dfb.hpp"
//#include "ezbc.hpp"

mat load_mat(IplImage* img);
mat loadMat(cv::Mat img);

#endif
