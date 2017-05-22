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
 * @file	psnr.hpp
 *
 * @brief	PSNR implementation.
 */

#ifndef PSNR_H
#define PSNR_H

#include "tool.hpp"

class PSNR {
public:
	/**
	 * PSNR
	 * @param input1 - source file1
	 * @param input2 - source file2
	 */
	PSNR(string input1, string input2) {
		cv::Mat i1 = cv::imread(input1).reshape(1);
		cv::Mat i2 = cv::imread(input2).reshape(1);

		i1.convertTo(I1, CV_8U);
		i2.convertTo(I2, CV_8U);

		if (I1.empty() || I2.empty()) {
			cerr << "Couldn't read the image.";
			exit(1);
		}

	}
	/**
	 * PSNR
	 * @param input1 - cv::Mat 1.
	 * @param input2 - cv::Mat 2.
	 */
	PSNR(cv::Mat input1, cv::Mat input2) {
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
