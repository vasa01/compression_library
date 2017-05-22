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
 * @file	ssim.cpp
 *
 * @brief	ssim implementation.
 */

#include "ssim.hpp"

#define C1 (float) (0.01 * 255 * 0.01  * 255)
#define C2 (float) (0.03 * 255 * 0.03  * 255)

/**
 * sigma calc
 * @param m
 * @param i
 * @param j
 * @param block_size
 * @return
 */
double sigma(cv::Mat &m, int i, int j, int block_size)
	{
	double sd = 0;

	cv::Mat m_tmp = m(cv::Range(i, i + block_size), cv::Range(j, j + block_size));
	cv::Mat m_squared(block_size, block_size, CV_64F);

	multiply(m_tmp, m_tmp, m_squared);

	double avg = mean(m_tmp)[0];
	double avg_2 = mean(m_squared)[0];


	sd = sqrt(avg_2 - avg * avg);

	return sd;
	}


/**
 * Covariance
 * @param m1
 * @param m2
 * @param i
 * @param j
 * @param block_size
 * @return
 */
double cov(cv::Mat &m1, cv::Mat &m2, int i, int j, int block_size){
	cv::Mat m3 = cv::Mat::zeros(block_size, block_size, m1.depth());
	cv::Mat m1_tmp = m1(cv::Range(i, i + block_size), cv::Range(j, j + block_size));
	cv::Mat m2_tmp = m2(cv::Range(i, i + block_size), cv::Range(j, j + block_size));


	multiply(m1_tmp, m2_tmp, m3);

	double avg_ro = mean(m3)[0]; // E(XY)
	double avg_r = mean(m1_tmp)[0]; // E(X)
	double avg_o = mean(m2_tmp)[0]; // E(Y)


	double sd_ro = avg_ro - avg_o * avg_r; // E(XY) - E(X)E(Y)

	return sd_ro;
}

/**
 * get ssim value
 * @return
 */
double SSIM::get2() {
	double ssim = 0;
	bool show_progress = false;
	int block_size = 4;
	int nbBlockPerHeight = I1.rows / block_size;
	int nbBlockPerWidth = I2.cols / block_size;

	for(int k = 0; k < nbBlockPerHeight; k++)
		{
		for(int l = 0; l < nbBlockPerWidth; l++)
			{
			int m = k * block_size;
			int n = l * block_size;

			double avg_o = mean(I1(cv::Range(k, k + block_size), cv::Range(l, l + block_size)))[0];
			double avg_r = mean(I2(cv::Range(k, k + block_size), cv::Range(l, l + block_size)))[0];
			double sigma_o = sigma(I1, m, n, block_size);
			double sigma_r = sigma(I2, m, n, block_size);
			double sigma_ro = cov(I1, I2, m, n, block_size);

			ssim += ((2 * avg_o * avg_r + C1) * (2 * sigma_ro + C2)) / ((avg_o * avg_o + avg_r * avg_r + C1) * (sigma_o * sigma_o + sigma_r * sigma_r + C2));

			}
		// Progress
		if(show_progress)
			cout << "\r>>SSIM [" << (int)((((double)k) / nbBlockPerHeight) * 100) << "%]";
		}
	ssim /= nbBlockPerHeight * nbBlockPerWidth;

	if(show_progress)
		{
		cout << "\r>>SSIM [100%]" << endl;
		cout << "SSIM : " << ssim << endl;
		}

	return ssim;
}
/**
 * get ssim scalar value
 * @return
 */
cv::Scalar SSIM::getMSSIM()
{

	cv::Mat I2_2 = I2.mul(I2);        // I2^2
	cv::Mat I1_2 = I1.mul(I1);        // I1^2
	cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2
								 

	cv::Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

	cv::Mat mu1_2 = mu1.mul(mu1);
	cv::Mat mu2_2 = mu2.mul(mu2);
	cv::Mat mu1_mu2 = mu1.mul(mu2);

	cv::Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	cv::Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	cv::Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	cv::Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}