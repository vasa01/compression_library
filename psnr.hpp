#ifndef PSNR_H
#define PSNR_H

#include "tool.hpp"

class PSNR {
public:
	PSNR::PSNR(string input1, string input2) {
		cv::Mat i1 = cv::imread(input1).reshape(1);
		cv::Mat i2 = cv::imread(input2).reshape(1);

		i1.convertTo(I1, CV_8U);
		i2.convertTo(I2, CV_8U);

		if (I1.empty() || I2.empty()) {
			cerr << "Couldn't read the image.";
			exit(1);
		}

	}
	PSNR::PSNR(cv::Mat input1, cv::Mat input2) {
		if (input1.empty() || input1.empty()) {
			cerr << "Empty input Mat image.";
			exit(1);
		}
		I1 = input1.clone();
		I2 = input2.clone();

		I1.convertTo(I1, CV_8U);
		I2.convertTo(I2, CV_8U);
	}
	double getPSNR();
private:
	cv::Mat I1, I2;
	double mse();
};

#endif