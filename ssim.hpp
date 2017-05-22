#ifndef SSIM_H
#define SSIM_H

#include "tool.hpp"

#define C1 (6.5025);
#define C2 (58.5225);

class SSIM{
public:
	SSIM::SSIM(string input1, string input2) {
		cv::Mat i1 = cv::imread(input1);
		cv::Mat i2 = cv::imread(input2);
		
		i1.convertTo(I1, CV_32F);          		
		i2.convertTo(I2, CV_32F);

		if (I1.empty() || I2.empty()){
			cerr << "Couldn't read the image";
			exit(1);
		}

	}
	SSIM::SSIM(cv::Mat input1, cv::Mat input2) {
		if (input1.empty() || input1.empty()) {
			cerr << "Empty input Mat image.";
			exit(1);
		}
		I1 = input1.clone();
		I2 = input2.clone();

		I1.convertTo(I1, CV_32F);
		I2.convertTo(I2, CV_32F);
	}
	cv::Scalar getMSSIM();
	double SSIM::get2();
private:
	cv::Mat I1, I2;
};


#endif
