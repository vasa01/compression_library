
#include "psnr.hpp"

double PSNR::getPSNR() {
	cv::Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	cv::Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if(false && sse <= 1e-10) // for small values return zero
		return 0;
	else
		{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255 * 255) / mse);
		return psnr;
	}
	//return 10 * log10((255 * 255) / mse());
}

double PSNR::mse() {
	double difference = 0;
	double sigma = 0;
	double mse;
	for (int i = 0; i<I1.rows; i++)
	{
		for (int j = 0; j<I1.cols; j++)
		{
			(double)difference = (double)I1.at<uchar>(i, j) - (double)I2.at<uchar>(i, j);
			sigma = sigma + (double)difference*(double)difference;
		}
	}
	mse = sigma / (I1.rows*I1.cols);
	return mse;
}