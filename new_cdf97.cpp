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

#define a1 (-1.586134342)
#define a2 (-0.05298011854)
#define a3 (0.8829110762)
#define a4 (0.4435068522)

#define k1 1.1496043988602418
#define k2 (1/1.1496043988602418)

using namespace cv;
using namespace std;

class CDF97_NEW {
	int levels;
	//float* coefs;
	Mat image;
	vector<float> tempbank;
	vector<Mat> images;
public:
	CDF97_NEW(int levels, Mat& image);
	void forward2d(Mat& image);
	void inverse2d(Mat& image);
	double test();
	double diff(const Mat& test, const Mat& ref);
private:
	void cv_dwt_cdf97_2f(Mat &img, const int &channel, const Size &size, int &j);
	void inverse(Line<float> line);
	void forward(Mat &img, Size size, int &j);
	void predict(Line<float> line, float coef);
	void update(Line <float> line, float coef);
};

CDF97_NEW::CDF97_NEW(int level, Mat& image) {
	this->levels = level;
	this->image = image;
}

void CDF97_NEW::forward(Mat &img, Size size, int &j){
	for (int c = 0; c < img.channels(); c++)
		cv_dwt_cdf97_2f(img, c, size, j);

}

void CDF97_NEW::cv_dwt_cdf97_2f(Mat &img, const int &channel, const Size &size, int &j) {

}