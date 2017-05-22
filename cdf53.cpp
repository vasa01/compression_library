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
 * @file	cdf53.cpp
 *
 * @brief	Implements the cdf 53 class.
 */

#include "cdf53.hpp"


#define WAIT_DELAY 30

#define DWT_VISUAL 0

/**
 * create cdf53
 * @param level
 * @param image
 * @param block_size
 */
CDF53::CDF53(int level, cv::Mat& image, int quant, int block_size) {
	this->levels = level;
	this->image = image;
	this->block_size = block_size;
	this->countChannels = image.channels();
	this->quant = quant;
}


/**
 * getLevelsObject
 * @return
 */
DwtLevels* CDF53::getLevelsObject() {
	return dwtLevels;
	}

/**
 * predict
 * @param line
 * @param coef1
 * @param coef2
 */
void CDF53::predict(Line<float> line, double coef1, double coef2) {
	/*for(size_t i = 1;i< line.size() -2 ;i = i+2){
	line[i] += coef * ( line[i-1] + line[i+1]);
	}
	line[line.size()-1] += 2 * coef * line[line.size() - 2];*/
	//
	for(int i = 1; i<line.size() - 2 + (line.size() & 1); i += 2)
		line[i] -= coef1 * (line[i - 1] + line[i + 1]);

	if(line.size() & 1)
		line[line.size() - 1] += 2 * coef2 * line[line.size() - 2];
	else
		line[line.size() - 1] -= 2 * coef1 * line[line.size() - 2];
	}

/**
 * update
 * @param line
 * @param coef
 */
void CDF53::update(Line<float> line, double coef) {
	/*for(size_t i = 2;i< line.size();i = i+2){
	line[i] += coef * ( line[i-1] + line[i+1]);
	}
	line[0] += 2 * coef * line[1];*/
	//

	line[0] += 2 * coef * line[1];

	for(int i = 2; i<line.size() - (line.size() & 1); i += 2)
		line[i] += coef * (line[i - 1] + line[i + 1]);

	}

/**
 * forward
 * @param line
 */
void CDF53::forward(Line<float> line) {
	assert(line.size() % 2 == 0);

	predict(line, p1, u1);

	update(line, u1);

	//predict(line, a3, a4);

	//update(line, a4);

	tempbank.resize(line.size());


	//double scale = 1.0f / k1;
	for(size_t i = 0; i < line.size(); ++i) {
		if(i % 2) {
			tempbank[i / 2 + line.size() / 2] = line[i];
			}
		else {
			tempbank[i / 2] = line[i];
			}
	}

	for(size_t i = 0; i < line.size(); i++) {
		line[i] = tempbank[i];
		}
	}

/**
 * inverse
 * @param line
 */
void CDF53::inverse(Line<float> line) {
	assert(line.size() % 2 == 0);

	tempbank.resize(line.size());

	//double scale = k1;
	for(size_t i = 0; i < line.size() / 2; i++) {
		tempbank[i * 2] = line[i];
		tempbank[i * 2 + 1] = line[i + line.size() / 2];
	}

	for(size_t i = 0; i < line.size(); i++) {
		line[i] = tempbank[i];
	}


	update(line, -u1);

	predict(line, -p1, -u1);
}

/**
 * inverse2 - test
 * @param line
 */
void CDF53::inverse2(Line<float> line) {
	assert(line.size() % 2 == 0);

	tempbank.resize(line.size());

	double scale = k1;
	for(size_t i = 0; i < line.size() / 2; i++) {
		tempbank[i * 2] = line[i] / scale;
		tempbank[i * 2 + 1] = line[i] / scale + 1;
		//tempbank[i * 2 + 1] = line[i + line.size() / 2] * scale;
		}

	for(size_t i = 0; i < line.size(); i++) {
		line[i] = tempbank[i];
		}

	/*update(line, -a4);

	predict(line, -a3, -a4);

	update(line, -a2);

	predict(line, -a1, -a2);*/
	}


//void printLine(Line<float> line) {
//	for(size_t i = 0; i < line.size(); i++) {
//		if(i == 0)
//			std::cout << line[i];
//		else
//			std::cout << ", " << line[i];
//		}
//	std::cout << ";" << endl;
//	}

/**
 * forward 2d
 * @param image
 */
void CDF53::forward2d(cv::Mat& image) {
	//const int c = (int)image.channels();	
	this->dwtLevels = new DwtLevels(levels, countChannels, block_size, quant);
	cv::Mat *channel = new cv::Mat[countChannels];
	split(image, channel);
	//cout << countChannels << endl;
	for(int c = 0; c < countChannels; c++) {
		channel[c].convertTo(channel[c], CV_32FC1);
		cv::Mat subImage(channel[c], cv::Rect(0, 0, channel[c].cols, channel[c].rows));
		cv::Mat tmp, tmp2;
		for(int k = 0; k < levels; ++k) {//levels
										 //cout<< k <<endl;

			for(int i = 0; i < subImage.rows; ++i) {
				Line<float> line(subImage.ptr<float>(i), subImage.cols);
				if(i % 10 == 0 && DWT_VISUAL) {
					subImage.convertTo(tmp2, CV_8UC1);
					imshow(Tool::IntToString(k) + "TMP2", tmp2);
					cv::waitKey(50);
					}
				forward(line);
				}

			cv::Mat transposed = subImage.t();

			for(int i = 0; i < transposed.rows; ++i) {
				Line<float> line(transposed.ptr<float>(i), transposed.cols);
				if(i % 10 == 0 && DWT_VISUAL) {
					transposed.convertTo(tmp2, CV_8UC1);
					imshow(Tool::IntToString(k) + "TMP2", tmp2);
					cv::waitKey(50);
					}
				forward(line);
				}
			cv::Mat(transposed.t()).copyTo(subImage);

			subImage.convertTo(tmp, CV_8UC1);
			/*if (k == 1) {
			cout << subImage << endl;
			}*/

			dwtLevels->setDwtImagesByLevelAndChannel(DwtImage(subImage, k, c, levels, countChannels));
			//cout << "seeeeeeeee" << dwtLevels->getMaxChanel() << endl;

			subImage.adjustROI(0, -(subImage.rows / 2), 0, -(subImage.cols / 2));

			}

		}

	//cout << images.size() << endl;
	merge(channel, countChannels, image);
	forwardMat = image.clone();


	}

/**
 * inverse 2d
 * @param image
 * @param minlevel
 */
void CDF53::inverse2d(cv::Mat& image, int minlevel) {
	//const int c = (int)image.channels();
	cv::Mat *channel = new cv::Mat[countChannels];
	split(image, channel);
	//levels -= minlevel;
	for(int i = 0; i < countChannels; i++) {
		channel[i].convertTo(channel[i], CV_32FC1);

		int factor = 1 << (levels - 1);
		cv::Mat unSubImage(channel[i], cv::Rect(cv::Point(0, 0), cv::Size(channel[i].cols / factor, channel[i].rows / factor)));
		for(int i = minlevel; i < levels; ++i) {

			cv::Mat transposed = unSubImage.t();
			for(int i = 0; i < transposed.rows; ++i) {
				Line<float> line(transposed.ptr<float>(i), transposed.cols);
				inverse(line);
				}

			cv::Mat(transposed.t()).copyTo(unSubImage);


			for(int i = 0; i < unSubImage.rows; ++i) {
				Line<float> line(unSubImage.ptr<float>(i), unSubImage.cols);
				inverse(line);
				}

			unSubImage.adjustROI(0, unSubImage.rows, 0, unSubImage.cols);
			}
		if(minlevel != 0) {
			for(int k = 0; k < minlevel; k++) {
				cv::Mat forResize = unSubImage;
				cv::Mat tmp, tmp2;
				resize(forResize(cv::Rect(0, 0, forResize.cols / 2, forResize.rows / 2)), tmp, cv::Size(), 2, 2, cv::INTER_CUBIC);
				tmp.convertTo(tmp2, CV_8UC1);
				imshow("scale", tmp2);
				cv::waitKey();
				channel[i] = tmp.clone();
				}
			}
		}
	merge(channel, countChannels, image);
	inverseMat = image.clone();
	}

//void CDF53::getSubBands(vector<cv::Mat>& sub, cv::Mat& image) {
//
//	cv::Mat LL(image, cv::Rect(0, 0, image.cols / 2, image.rows / 2));
//	sub.push_back(LL.clone());
//	cv::Mat HL(image, cv::Rect(0, 0, image.cols / 2, image.rows / 2));
//	sub.push_back(LL.clone());
//}

/**
 * get levels
 * @return
 */
vector<DwtImage> CDF53::getLevels() {
	assert(!dwtLevels->getIsEmpty());
	std::cout << "not empty" << endl;
	vector<DwtImage> result;

	for(int l = 0; l < dwtLevels->getMaxLevel(); l++) {
		for(int c = 0; c < dwtLevels->getMaxChanel(); c++) {
			result.push_back(dwtLevels->getDwtImagesByLevelAndChannel(l, c));
			}
		}
	return result;
	}







