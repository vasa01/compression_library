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
 * @file	ssim.hpp
 *
 * @brief	ssim implementation.
 */

#ifndef SSIM_H
#define SSIM_H

#include "tool.hpp"

#define C1 (6.5025);
#define C2 (58.5225);
/**
 * class SSIM
 */
class SSIM{
public:
	/**
	 * SSIM
	 * @param input1 - source file
	 * @param input2 - source file
	 */
	SSIM(string input1, string input2) {
		cv::Mat i1 = cv::imread(input1);
		cv::Mat i2 = cv::imread(input2);
		
		i1.convertTo(I1, CV_32F);          		
		i2.convertTo(I2, CV_32F);

		if (I1.empty() || I2.empty()){
			cerr << "Couldn't read the image";
			exit(1);
		}

	}
	/**
	 *
	 * @param input1 - cv::Mat
	 * @param input2 - cv::Mat
	 */
	SSIM(cv::Mat input1, cv::Mat input2) {
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
	double get2();
private:
	cv::Mat I1, I2;
};


#endif
