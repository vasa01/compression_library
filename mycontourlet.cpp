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
 * @file	mycontourlet.cpp
 *
 * @brief	Mycontourlet implementation.
*/
#include "mycountourlet.hpp"


/**
 * MyContourlet
 * @param contourlet
 */
MyContourlet::MyContourlet(contourlet_t *contourlet) {
	this->ct_levels = contourlet->ct_levels;
	this->wt_levels = contourlet->wt_levels;
	//double te = contourlet->H0[0];
	
	for (int i = 0; i < ct_levels; i++) {
		this->dfb_levels.push_back(contourlet->dfb_levels[i]);
	}
	this->low = saveMat(contourlet->low);
	
	for (int l = 0; l < ct_levels; l++) {
		std::vector<cv::Mat> tmp;
		for (int d = 0; d < 16; d++) {
			mat te = contourlet->high[l][d];
			//cout << "width: " << mat_width(te) << " height: " << mat_height(te) << endl;
			tmp.push_back(saveMat(te).clone());
		}
		this->high.push_back(tmp);
	}
	//std::cout << "sem" << std::endl;
	for (int i = 0; i < 9; i++) {
		this->H0.push_back(contourlet->H0[i]);
		this->G0.push_back(contourlet->G0[i]);
	}

}

/**
 * MyContourlet
 * @param _high - high cv::Mat
 * @param _low - low cv::Mat
 * @param ct_levels - int level
 */
MyContourlet::MyContourlet(std::vector<std::vector<cv::Mat>> _high, cv::Mat _low, int ct_levels) {
	this->ct_levels = ct_levels;
	this->low = _low.clone();

	for (int l = ct_levels-1; l >-1; l--) {
		std::vector<cv::Mat> tmp;
		for (int d = 0; d < 16; d++) {
			cv::Mat te = _high[l][d];
			tmp.push_back(te);
		}
		this->high.push_back(tmp);
	}

}

/**
 * MyContourlet
 * @param _high - high Matrix
 * @param _low - low Matrix
 * @param ct_levels - int level
 */
MyContourlet::MyContourlet(std::vector<std::vector<Matrix>> _high, Matrix _low, int ct_levels) {
	this->ct_levels = ct_levels;
	this->low = _low.getCvMat();

	for (int l = ct_levels-1; l > -1; l--) {
		std::vector<cv::Mat> tmp;
		for (int d = 0; d < 16; d++) {
			Matrix te = _high[l][d];
			tmp.push_back(te.getCvMat());
		}
		this->high.push_back(tmp);
	}

}

/**
 * getBlocks for encode
 * @param block_size
 * @return - vector DwtImage
 */
vector<DwtImage*> MyContourlet::getBlocks(int block_size) {
	vector<DwtImage*> vectorImages2;
	

	if (low.cols > block_size || low.rows > block_size) {
		for (int y = 0; y < low.rows; y += block_size){
			for (int x = 0; x < low.cols; x += block_size){
				int tempWidth = block_size;
				int tempHeight = block_size;
				if (y + tempHeight > low.rows) {
					tempHeight = low.rows - y;
				}
				if (x + tempWidth > low.cols) {
					tempWidth = low.cols - x;
				}
				cv::Rect rect = cv::Rect(x, y, tempWidth, tempHeight);
				vectorImages2.push_back(new DwtImage(cv::Mat(low, rect), 0 , 0, ct_levels, 1,Bands::LL));
					/*smallImages.push_back(cv::Mat(image, rect));
					imshow("smallImages", cv::Mat(image, rect));
					waitKey(0);*/
			}
		}
	}
	else {
		vectorImages2.push_back(new DwtImage(low, 0, 0, ct_levels, 1, Bands::LL));
	}

	int l, d;
	for (l = this->high.size()-1; l > -1 ; l--) {
		for (int d = 0; d < high[l].size(); d++) {
			cv::Mat image = high[l][d];
			//cout << "highH: " << image.rows << " highW: " << image.cols << endl;
			if (image.cols > block_size || image.rows > block_size) {
				for (int y = 0; y < image.rows; y += block_size) {
					for (int x = 0; x < image.cols; x += block_size) {
						int tempWidth = block_size;
						int tempHeight = block_size;
						if (y + tempHeight > image.rows) {
							tempHeight = image.rows - y;
						}
						if (x + tempWidth > image.cols) {
							tempWidth = image.cols - x;
						}
						cv::Rect rect = cv::Rect(x, y, tempWidth, tempHeight);
						vectorImages2.push_back(new DwtImage(cv::Mat(image, rect), l+1, d+1, ct_levels, 1, Bands::LL));
						/*smallImages.push_back(cv::Mat(image, rect));
						imshow("smallImages", cv::Mat(image, rect));
						waitKey(0);*/
					}
				}
			}
			else {
				vectorImages2.push_back(new DwtImage(image, l+1, d+1, ct_levels, 1, Bands::LL));
			}
		}
	}


	

	return vectorImages2;
}

/**
 * Get struct countourlet_t
 * @return - countourlet_t
 */
contourlet_t * MyContourlet::getStruct() {
	//cout << "get struct" << endl;
	contourlet_t *ct;

	ivec dfb_levels;
	int l;

	dfb_levels = ivec_new(ct_levels);
	for (int i = 0; i < ct_levels; i++) {
		dfb_levels[i] = 4;
	}
	/*dfb_levels[0] = 4;
	dfb_levels[1] = 4;
	dfb_levels[2] = 4;*/

	ct = (contourlet_t *)malloc(sizeof(contourlet_t));
	ct->ct_levels = ct_levels;
	ct->dfb_levels = ivec_clone(dfb_levels);
	ct->high = (double ****)calloc(ct_levels, sizeof(mat *));
	///
	
	ct->low = loadMat(this->low);
	///
	/* pyramid filters */
	ct->H0 = 0;
	ct->G0 = 0;

	ct->H0 = vec_new_string(" 3.782845550699535e-02, -2.384946501937986e-02, -1.106244044184226e-01, 3.774028556126536e-01, 8.526986790094022e-01, 3.774028556126537e-01, -1.106244044184226e-01, -2.384946501937986e-02, 3.782845550699535e-02");
	ct->G0 = vec_new_string("-6.453888262893856e-02, -4.068941760955867e-02, 4.180922732222124e-01, 7.884856164056651e-01, 4.180922732222124e-01, -4.068941760955867e-02, -6.453888262893856e-02");
	//std::cout << "sem uvnitr " << ct_levels << std::endl;
	assert(ct->H0);
	assert(ct->G0);

	/*for (int i = 0; i < 9; i++) {
		ct->H0[i] = this->H0[i];
		ct->G0[i] = this->G0[i];
	}*/
	for (l = 0; l < ct_levels; l++) {
		/* allocate directionnal subbands for that level */
		ct->high[l] = (mat *)calloc(1 << ct->dfb_levels[l], sizeof(mat));
	}
	//std::cout << "semka az" << ct_levels << std::endl;
	/* allocate directionnal subbands */
	for (l = 0; l < this->high.size(); l++) {
		for (int d = 0; d < high[l].size(); d++) {
			//IplImage tmp = this->high[l][d];
			//cout << "width: " << high[l][d].cols << " height: " << high[l][d].rows << endl;
			ct->high[l][d] = loadMat(this->high[l][d]);
		}
	}



	return(ct);
}