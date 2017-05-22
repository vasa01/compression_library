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
 * @file	joinSubimage.cpp
 *
 * @brief	joinSubimage implementation.
 */

#ifndef JOIN_SUBIMAGE_H
#define JOIN_SUBIMAGE_H

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
#include "dwtImage.hpp"


#include "mat.hpp"
#include "contourlet.hpp"
#include "load_mat.hpp"
#include "save_mat.hpp"
#include "mycountourlet.hpp"

//using namespace cv;
using namespace std;

/**
 * splitVector
 * @tparam T
 * @param vec
 * @param n
 * @return - vector of vector
 */
template<typename T>
std::vector<std::vector<T>> splitVector(const std::vector<T>& vec, size_t n)
{
	std::vector<std::vector<T>> outVec;

	for(int i = 0; i < n; i++) {
		outVec.push_back(vector<T>());
	}
	//int counter=0;
	for(int i=0; i < vec.size();i=i+n) {
		int counter = i;
		for(int ii = 0; ii < n; ii++) {
			outVec[ii].push_back(vec[counter]);
			counter++;
		}
	}

	return outVec;
}

/**
 * class JoinSubimage
 */
class JoinSubimage {
public:
	/**
	 * create joinImage
	 * @param _width
	 * @param _height
	 * @param _maxLevel
	 * @param _maxChannel
	 * @param _block_size
	 */
	JoinSubimage(int _width, int _height, int _maxLevel, int _maxChannel, int _block_size, int q) :width(_width), height(_height), maxLevel(_maxLevel), maxChannel(_maxChannel), q(q) {
		minLevel = 1000;
		realMax = 0;
		this->block_size = _block_size;
	}


	/**
	 * add new block
	 * @param dwt
	 */
	void add(DwtImage dwt) {
		//cout << "Bands: " << (int)dwt.getTypeSubBands() << " Level: " << dwt.getLevel() << " Channel: " << dwt.getChannel() << endl;
		storage.push_back(dwt);
		if (dwt.getLevel() < minLevel) {
			minLevel = dwt.getLevel();
		}
		if(dwt.getLevel() > realMax) {
			realMax = dwt.getLevel();
		}
	}
	int size() {
		return storage.size();
	}

	/**
	 * join conturlet
	 * @param input
	 * @param height2
	 * @param width2
	 * @return - image
	 */
	cv::Mat joinContur(std::vector<cv::Mat> input, int height2, int width2) {
		cv::Mat result = cv::Mat(height2, width2, CV_32FC1);
		//cout << "in highH: " << height2 << " highW: " << width2 << endl;
		/*for (int i = 0; i < input.size(); i++) {
			cv::Mat temp = input[i];
		}*/
		int index = 0;
		if (width2 > block_size || height2 > block_size) {
			for (int y = 0; y < height2; y += block_size) {
				for (int x = 0; x < width2; x += block_size) {
					/*int tempWidth = 32;
					int tempHeight = 32;
					if (y + tempHeight > low.rows) {
						tempHeight = low.rows - y;
					}
					if (x + tempWidth > low.cols) {
						tempWidth = low.cols - x;
					}*/
					if (index >= input.size()) {
						cout << "index mox velky" << endl;
						exit(1);
					}
					cv::Rect rect = cv::Rect(x, y, input[index].cols, input[index].rows);
					input[index].copyTo(result(rect));
					//vectorImages2.push_back(new DwtImage(cv::Mat(low, rect), 0, 0, ct_levels, 1, Bands::LL));
					index++;
					/*smallImages.push_back(cv::Mat(image, rect));
					imshow("smallImages", cv::Mat(image, rect));
					waitKey(0);*/
				}
			}
		}
		else {
			result = input[0];
			//vectorImages2.push_back(new DwtImage(low, 0, 0, ct_levels, 1, Bands::LL));
		}
		//cout << "uvnitr" << endl;
		return result;
	}

	/**
	 * get join image without subblock
	 * @param info
	 * @return
	 */
	cv::Mat getJoinImage2(vector<int> info) {
		int lowWidth = info[0];
		int lowHeight = info[1];
		int highW = info[2];
		int highH = info[3];
		int highSize = info[4];
		int highhighSize = info[5];

		for (int i = 0; i < highSize-1; i++) {
			highH /= 2;
			highW /= 2;
		}
		int tempHighW = highW;
		int tempHighH = highH;
		cv::Mat result;
		
		cv::Mat *channel = new cv::Mat[maxChannel];

		vector<vector<DwtImage>> stor = splitVector(storage, maxChannel);
		//cout << "stor size: " << stor.size() << "_"<< stor[0].size()<< endl;
		for (int i = 0; i < maxChannel; i++) {
			cv::Mat low = joinContur(getByLevelAndChannel(stor[i], 0, 0), lowHeight, lowWidth);
			/*cv::Mat tmp2;
			low.convertTo(tmp2, CV_8UC1);
			cv::imshow("low dec", tmp2);
			cv::waitKey();*/

			//cout << "low" << endl;
			std::vector<std::vector<cv::Mat>> high;
			int l, d;
			for (l = highSize-1; l >-1; l--) {
				//cout << "highH: " << highH <<" highW: "<< highW << endl;
				std::vector<cv::Mat> temp;
				for (int d = 0; d < highhighSize; d++) {
					//cv::Mat image = high[l+1][d+1];
					if (d < highhighSize / 2) {
						temp.push_back(joinContur(getByLevelAndChannel(stor[i], l + 1, d + 1), highH, highW));
					}
					else {
						temp.push_back(joinContur(getByLevelAndChannel(stor[i], l + 1, d + 1), highW, highH));
					}
				}
				high.push_back(temp);
				highW *= 2;
				highH *= 2;
			}
			MyContourlet mycon = MyContourlet(high, low, high.size());
			//cout << "sem"<<i << endl;
			/*cv::Mat tmp2;
			high[0][0].convertTo(tmp2, CV_8UC1);
			cv::imshow("low dec", tmp2);
			cv::waitKey();*/

			mat dest2 = mat_new(width, height);
			contourlet_t *contourlet2;
			//cout << "sekkkk!!" << endl;
			contourlet2 = mycon.getStruct();
			//cout << high[0][0].rows << endl;
			//cout << "sekkkk! w:"<<width << " height: "<<height << endl;
			contourlet_itransform(contourlet2, dest2);
			//cout << "sekkkk*" << endl;
			channel[i] = saveMat(dest2);
			
			contourlet_delete(contourlet2);
			mat_delete(dest2);
			highW = tempHighW;
			highH = tempHighH;
		}

		if (maxChannel == 3) {
			cv::Mat test;
			merge(channel, maxChannel, test);
			result = test.clone();
			/*test.convertTo(test, CV_8UC1);
			imshow("EBCOT CON JOIN", test);
			cv::waitKey();*/
		}
		else {
			result = channel[0].clone();
		}
		return result;
	
	}

	/**
	 * get join image by dwt sizes
	 * @return - join image
	 */
	cv::Mat getJoinImage() {
		cv::Mat result;
		if (maxChannel == 3) {
			result = cv::Mat(height, width, CV_32FC3);
		}
		else {
			result = cv::Mat(height, width, CV_32FC1);
		}

		if(q){
		for(int i = 0; i < storage.size(); i++) {
			DwtImage dwt = storage[i];
			cv::Mat edit = quant(dwt.getImage(),dwt.getLevel(),dwt.getMaxLevel());
			storage[i].setImage(edit);
		}

		}
		
		joinEqually();

		//DwtImage dwt;
	
		//imshow("test", get(Bands::LL, 1, 0).getImage());
		//waitKey();
		DwtImage temp;
		for (int l = realMax ; l > -1; l--) {//maxLevel - 1
			if (true) {
				vector<cv::Mat> vecLL;
				vector<cv::Mat> vecHL;
				vector<cv::Mat> vecLH;
				vector<cv::Mat> vecHH;
				for (int ch = 0; ch < maxChannel; ch++) {
					vecLL.push_back(get(Bands::LL, l, ch).getImage());
					vecHL.push_back(get(Bands::HL, l, ch).getImage());
					vecLH.push_back(get(Bands::LH, l, ch).getImage());
					vecHH.push_back(get(Bands::HH, l, ch).getImage());
				}
				cv::Mat LL, HL, LH, HH;
				merge(vecLL, LL);
				merge(vecHL, HL);
				merge(vecLH, LH);
				merge(vecHH, HH);

				/*cv::Mat tmp2;
				LL.convertTo(tmp2, CV_8UC1);
				imshow("LL", tmp2);
				cv::waitKey();*/

				int w = LL.cols;
				int h = LL.rows;

				cv::Mat newMat;
				if (maxChannel == 3) {
					newMat = cv::Mat(h * 2, w * 2, CV_32FC3);
				}
				else {
					newMat = cv::Mat(h * 2, w * 2, CV_32FC1);
				}
				

				//cout << LH.cols << endl;
				//cout << "sem " << endl;
				LL.copyTo(newMat(cv::Rect(0, 0, h, w)));
				//cout << "sem " << endl;
				LH.copyTo(newMat(cv::Rect(0, w, h, w)));
				//cout << "sem " << endl;
				HL.copyTo(newMat(cv::Rect(h, 0, h, w)));
				//cout << "sem " << endl;
				HH.copyTo(newMat(cv::Rect(h, w, h, w)));
				if (l == 0 || l == minLevel) {
					if (newMat.size() != result.size()) {
						newMat.copyTo(result(cv::Rect(0, 0, newMat.rows, newMat.cols)));
						return result;
					}
					return newMat;
				}
				cv::Mat *channel = new cv::Mat[maxChannel];
				split(newMat, channel);

				for (int ch = 0; ch < maxChannel; ch++) {
					add(DwtImage(channel[ch], l - 1, ch, maxLevel, maxChannel,Bands::LL));
				}
			}
		}

		

		return result;
		
	}
private:
	vector<DwtImage> storage;
	int width;
	int height;
	int maxLevel;
	int maxChannel;
	int minLevel;
	int realMax;
	int block_size;
	int q;

	cv::Mat quant(cv::Mat mat, int level, int maxLevel) {
		cv::Mat result = mat.clone();

		result *= (1.25*(level+1));

		return result;
	}
	/**
	 * getByLevelAndChannel
	 * @param input
	 * @param level
	 * @param channel
	 * @return - vector cv::Mat
	 */
	std::vector<cv::Mat> getByLevelAndChannel(std::vector<DwtImage> input, int level, int channel) {
		std::vector<cv::Mat> result;

		for (int i = 0; i < input.size(); i++) {
			DwtImage temp = input[i];
			if (temp.getChannel() == channel && temp.getLevel() == level) {
				result.push_back(temp.getImage());
			}
		}
		//cout << result.size() << endl;
		return result;
	}

	/**
	 * clear find in storage
	 * @param find
	 */
	void clear(DwtImage find) {
		std::vector<DwtImage> temp;
		for (int i = 0; i < storage.size(); i++) {
			if (find.getTypeSubBands() == storage[i].getTypeSubBands() &&
				find.getLevel() == storage[i].getLevel() &&
				find.getChannel() == storage[i].getChannel()) {
				continue;
			}
			else {
				temp.push_back(storage[i]);
			}
		}
		this->storage.clear();
		this->storage = temp;
	}

	/**
	 * join subblock
	 */
	void joinEqually() {
		//cout << "size: " << storage.size() << endl;
		while (1) {
			std::vector<DwtImage> temp;
			for (int i = 0; i < storage.size(); i++) {
				DwtImage find = storage[i];
				for (int k = 0; k < storage.size(); k++) {
					if (find.getTypeSubBands() == storage[k].getTypeSubBands() &&
						find.getLevel() == storage[k].getLevel() &&
						find.getChannel() == storage[k].getChannel()) {
						temp.push_back(storage[k]);
					}
				}
				if (temp.size() > 1) {
					break;
				}
				else {
					temp.clear();
				}
			}
			if (temp.size() < 2) {
				break;
			}
			int factor = sqrt(temp.size());
			cv::Mat join = cv::Mat(factor*temp[0].h(), factor*temp[0].h(), CV_32FC1);
			//cout << "newMat: " << join.cols << " " << join.rows<< " "<< temp.size()<< " factor: "<< factor << endl;
			int index = 0;
			for (int y = 0; y < join.rows; y += block_size){
				for (int x = 0; x < join.cols; x += block_size){
					cv::Rect rect = cv::Rect(x, y, block_size, block_size);
					temp[index].getImage().copyTo(join(rect));
					index++;
				}
			}
			DwtImage newDwt = DwtImage(join,temp[0].getLevel(),temp[0].getChannel(),temp[0].getMaxLevel(),temp[0].getMaxChannel(),temp[0].getTypeSubBands());

			clear(temp[0]);
			temp.clear();
			storage.push_back(newDwt);
		}
	}

	/**
	 * get block by band and level and channel
	 * @param band
	 * @param level
	 * @param channel
	 * @return
	 */
	DwtImage get(Bands band, int level, int channel) {
		//cout << "Bands: " << (int)band << " Level: " << level << " Channel: " << channel << endl;
		for (int i = 0; i < storage.size(); i++) {
			DwtImage val = storage[i];
			if (val.getChannel() == channel && val.getTypeSubBands() == band && val.getLevel() == level) {
				return val;
			}
			else if (val.getChannel() == channel && val.getTypeSubBands() == Bands::HLLHHH && val.getLevel() == level) {
				cv::Mat tmp = val.getImage();
				if (band == Bands::HL) {
					return DwtImage(tmp(cv::Rect(0, 0,tmp.cols/3 , tmp.rows)),val.getLevel(),val.getChannel(),maxLevel,maxChannel,Bands::HL);
				}else if (band == Bands::LH) {
					return DwtImage(tmp(cv::Rect(0, tmp.cols / 3, tmp.cols / 3, tmp.rows)), val.getLevel(), val.getChannel(), maxLevel, maxChannel, Bands::LH);
				}else if (band == Bands::HH) {
					return DwtImage(tmp(cv::Rect(0, (tmp.cols / 3)*2, tmp.cols / 3, tmp.rows)), val.getLevel(), val.getChannel(), maxLevel, maxChannel, Bands::HH);
				}
				else {
					cout << "This image doesnt exist split" << endl;
					exit(1);
				}
			}
		}
		cout << "This image doesnt exist" << endl;
		exit(1);
	}
};

#endif
