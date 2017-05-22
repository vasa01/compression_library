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
 * @file	psnr.cpp
 *
 * @brief	PSNR implementation.
 */

#include "psnr.hpp"

/**
 * getPSNR
 * @return  - psnr value
 */
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

/**
 * MSE
 * @return MSE value
 */
double PSNR::mse() {
	double difference = 0;
	double sigma = 0;
	double mse;
	for (int i = 0; i<I1.rows; i++)
	{
		for (int j = 0; j<I1.cols; j++)
		{
			difference = (double)I1.at<uchar>(i, j) - (double)I2.at<uchar>(i, j);
			sigma = sigma + (double)difference*(double)difference;
		}
	}
	mse = sigma / (I1.rows*I1.cols);
	return mse;
}
