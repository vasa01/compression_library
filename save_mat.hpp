#ifndef SAVE_MAT_H
#define SAVE_MAT_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <time.h>
#include "io.hpp"
#include "distance.hpp"
#include <stdio.h>
#include <math.h>

#include "mat.hpp"
#include "load_mat.hpp"


IplImage * save_mat(mat m);
cv::Mat saveMat(mat m);
#endif