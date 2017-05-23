/**********************************************************************************************//**
 * @file	dwtLevels.hpp
 *
 * @brief	Declares the dwt levels class.
 **************************************************************************************************/

#ifndef DWT_LEVELS_H
#define DWT_LEVELS_H

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
#include <deque>
#include "dwtImage.hpp"
//#include "cdf97.hpp"

//using namespace cv;
//using namespace std;

/**********************************************************************************************//**
 * @class	DwtLevels
 *
 * @brief	A dwt levels.
 *
 * @author	Vasa 0
 * @date	06.01.2017
 **************************************************************************************************/

class DwtLevels {
private:
	//DwtImage **images;
	std::vector< std::vector<DwtImage>> images;
	int levels;
	int maxLevel;
	int maxChannel;
	bool isEmpty;
	int block_size;
	int q;

	cv::Mat quant(cv::Mat mat, int level, int maxLevel) {
		cv::Mat result = mat.clone();

		result /= (1.25*((level/2)+1));

		return result;
	}
public:

	/**********************************************************************************************//**
	 * @fn	DwtLevels::DwtLevels( int maxLevel, int maxChannel)
	 *
	 * @brief	Constructor.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	maxLevel  	The maximum level.
	 * @param	maxChannel	The maximum channel.
	 **************************************************************************************************/

	DwtLevels( int maxLevel, int maxChannel, int block_size, int q) {
		this->maxChannel = maxChannel;
		this->maxLevel = maxLevel;
		this->block_size = block_size;
		this->q =q;
		/*images = new DwtImage*[maxLevel];
		for (int i = 0; i < maxLevel; ++i)
			images[i] = new DwtImage[maxChannel];*/
		for(int i = 0; i < maxLevel; i++){
			std::vector<DwtImage> temp;
			for(int ii=0; ii < maxChannel; ii++){
				temp.push_back(DwtImage());			
			}
			images.push_back(temp); 
		}
		isEmpty = true;
	};

	/**********************************************************************************************//**
	 * @fn	DwtImage DwtLevels::getDwtImagesByLevelAndChannel(int level, int channel)
	 *
	 * @brief	Gets dwt images by level and channel.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	level  	The level.
	 * @param	channel	The channel.
	 *
	 * @return	The dwt images by level and channel.
	 **************************************************************************************************/

	DwtImage getDwtImagesByLevelAndChannel(int level, int channel) {
		return images[level][channel];
	}

	/**********************************************************************************************//**
	 * @fn	void DwtLevels::setDwtImagesByLevelAndChannel(DwtImage dwtImage, int level, int channel)
	 *
	 * @brief	Sets dwt images by level and channel.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	dwtImage	The dwt image.
	 * @param	level   	The level.
	 * @param	channel 	The channel.
	 **************************************************************************************************/

	void setDwtImagesByLevelAndChannel(DwtImage dwtImage, int level, int channel) {
		images[level][channel] = dwtImage;
		isEmpty = false;
	}

	/**********************************************************************************************//**
	 * @fn	void DwtLevels::setDwtImagesByLevelAndChannel(DwtImage dwtImage)
	 *
	 * @brief	Sets dwt images by level and channel.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	dwtImage	The dwt image.
	 **************************************************************************************************/

	void setDwtImagesByLevelAndChannel(DwtImage dwtImage) {
		images[dwtImage.getLevel()][dwtImage.getChannel()] = dwtImage;
		isEmpty = false;
	}

	/**********************************************************************************************//**
	 * @fn	bool DwtLevels::getIsEmpty()
	 *
	 * @brief	Gets is empty.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	True if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool getIsEmpty() {
		return isEmpty;
	}

	/**********************************************************************************************//**
	 * @fn	int DwtLevels::getMaxLevel()
	 *
	 * @brief	Gets maximum level.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The maximum level.
	 **************************************************************************************************/

	int getMaxLevel() {
		return maxLevel;
	}

	/**********************************************************************************************//**
	 * @fn	int DwtLevels::getMaxChanel()
	 *
	 * @brief	Gets maximum chanel.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The maximum chanel.
	 **************************************************************************************************/

	int getMaxChanel() {
		return maxChannel;
	}

	std::vector<DwtImage*> getImagesVec2() {
		std::vector<DwtImage*> vectorImages;

		std::vector<DwtImage*> LL;
		std::vector<DwtImage*> HL;
		std::vector<DwtImage*> LH;
		std::vector<DwtImage*> HH;

		for (int l = maxLevel - 1; l > -1; l--) {
			for (int ch = 0; ch < maxChannel; ch++) {
				try {
					std::vector<DwtImage*> temp = getDwtImagesByLevelAndChannel(l, ch).createBands();
					for (int i = 0; i < temp.size(); i++) {
						if (temp[i]->getLevel() < maxLevel - 1 && temp[i]->getTypeSubBands() == Bands::LL) {
							continue;
						}

						switch (temp[i]->getTypeSubBands()) {
						case Bands::LL:
							LL.push_back(temp[i]);
							break;
						case Bands::HL:
							HL.push_back(temp[i]);
							break;
						case Bands::LH:
							LH.push_back(temp[i]);
							break;
						case Bands::HH:
							HH.push_back(temp[i]);
							break;
						default:
							break;
						}
					}

				}
				catch (const std::exception&) {
					std::cout << "<-level: " << l << " channel: " << ch << std::endl;
				}

			}
			for (int i = 0; i < LL.size(); i++) {
				vectorImages.push_back(LL[i]);
			}
			LL.clear();

			for (int i = 0; i < HL.size(); i++) {
				vectorImages.push_back(HL[i]);
			}
			HL.clear();

			for (int i = 0; i < LH.size(); i++) {
				vectorImages.push_back(LH[i]);
			}
			LH.clear();

			for (int i = 0; i < HH.size(); i++) {
				vectorImages.push_back(HH[i]);
			}
			HH.clear();

		}
		return vectorImages;
	}

	std::vector<DwtImage*> getImagesVec3() {
		std::vector<DwtImage*> vectorImages;

		std::deque<DwtImage*> LL;
		std::deque<DwtImage*> HL;
		std::deque<DwtImage*> LH;
		std::deque<DwtImage*> HH;

		for (int l = maxLevel - 1; l > -1; l--) {
			for (int ch = 0; ch < maxChannel; ch++) {
				try {
					std::vector<DwtImage*> temp = getDwtImagesByLevelAndChannel(l, ch).createBands();
					for (int i = 0; i < temp.size(); i++) {
						if (temp[i]->getLevel() < maxLevel - 1 && temp[i]->getTypeSubBands() == Bands::LL) {
							continue;
						}

						switch (temp[i]->getTypeSubBands()) {
						case Bands::LL:
							LL.push_back(temp[i]);
							break;
						case Bands::HL:
							/*if(l == maxLevel - 1) {
								CDF97 cdf97 = CDF97(maxLevel-2,temp[i]->getImage());
								cdf97.forward2d(temp[i]->getImage());
								HL.push_back(new DwtImage(cdf97.getForward(),temp[i]->getLevel(),temp[i]->getChannel(),temp[i]->getMaxLevel(), temp[i]->getMaxChannel(),temp[i]->getTypeSubBands()));
							}else{*/
								HL.push_back(temp[i]);
							//}
							break;
						case Bands::LH:
							LH.push_back(temp[i]);
							break;
						case Bands::HH:
							HH.push_back(temp[i]);
							break;
						default:
							break;
						}
					}

				}
				catch (const std::exception&) {
					std::cout << "<-level: " << l << " channel: " << ch << std::endl;
				}

			}
			/*for (int i = 0; i < LL.size(); i++) {
				vectorImages.push_back(LL[i]);
			}
			LL.clear();

			for (int i = 0; i < HL.size(); i++) {
				vectorImages.push_back(HL[i]);
			}
			HL.clear();

			for (int i = 0; i < LH.size(); i++) {
				vectorImages.push_back(LH[i]);
			}
			LH.clear();

			for (int i = 0; i < HH.size(); i++) {
				vectorImages.push_back(HH[i]);
			}
			HH.clear();*/

		}
		
		
		while(1) {
			std::vector<cv::Mat> blocks64;
			for(int ch = 0; ch < maxChannel; ch++) {
				auto ll = LL.front();
				LL.pop_front();
				auto hl = HL.front();
				HL.pop_front();
				auto lh = LH.front();
				LH.pop_front();
				auto hh = HH.front();
				HH.pop_front();

				cv::Mat newMat = cv::Mat(ll->getImage().rows*2,ll->getImage().cols*2,CV_32FC1);

				int h = ll->getImage().rows;
				int w = ll->getImage().cols;

				ll->getImage().copyTo(newMat(cv::Rect(0, 0, h, w)));
				//cout << "sem " << endl;
				lh->getImage().copyTo(newMat(cv::Rect(0, w, h, w)));
				//cout << "sem " << endl;
				hl->getImage().copyTo(newMat(cv::Rect(h, 0, h, w)));
				//cout << "sem " << endl;
				hh->getImage().copyTo(newMat(cv::Rect(h, w, h, w)));

				DwtImage* dwtNew = new DwtImage(newMat,ll->getLevel()-1,ll->getChannel(),ll->getMaxLevel(),ll->getMaxChannel(),Bands::LL);
				
				LL.push_back(dwtNew);
			}
			if(LL[0]->getImage().cols == block_size && LL[0]->getImage().rows == block_size) {
				for(int i = 0; i < LL.size(); i++) {
					vectorImages.push_back(LL[i]);
				}
				LL.clear();

				for(int i = 0; i < HL.size(); i++) {
					vectorImages.push_back(HL[i]);
					vectorImages.push_back(LH[i]);
					vectorImages.push_back(HH[i]);
				}
				HL.clear();
				LH.clear();
				HH.clear();
				
				break;
			}
		}

		std::vector<DwtImage*> vectorImages2;
		for (int i = 0; i < vectorImages.size(); i++) {
			DwtImage* dwt = vectorImages[i];
			cv::Mat image = dwt->getImage();
			if (image.cols > block_size || image.rows > block_size) {
				for (int y = 0; y < image.rows; y += block_size)
				{
					for (int x = 0; x < image.cols; x += block_size)
					{
						int tempWidth = block_size;
						int tempHeight = block_size;
						if(y + tempHeight > image.rows) {
							tempHeight = image.rows - y;
						}
						if(x + tempWidth > image.cols) {
							tempWidth = image.cols - x;
						}
						cv::Rect rect = cv::Rect(x, y, tempWidth, tempHeight);
						vectorImages2.push_back(new DwtImage(cv::Mat(image, rect), dwt->getLevel(), dwt->getChannel(), dwt->getMaxLevel(), dwt->getMaxChannel(), dwt->getTypeSubBands()));
						/*smallImages.push_back(cv::Mat(image, rect));
						imshow("smallImages", cv::Mat(image, rect));
						waitKey(0);*/
					}
				}
			}
			else {
				vectorImages2.push_back(vectorImages[i]);
			}
		}

		std::vector<DwtImage*> snad;
		if(q){
		for(int i = 0; i < vectorImages2.size(); i++) {
			DwtImage* dwt = vectorImages2[i];
			cv::Mat edit = quant(dwt->getImage(), dwt->getLevel(), dwt->getMaxLevel());
			snad.push_back(new DwtImage(edit, dwt->getLevel(),dwt->getChannel(),dwt->getMaxLevel(), dwt->getMaxChannel(),dwt->getTypeSubBands()) );
		}
		}
		else{
			snad= vectorImages2;
		}


		return snad;
	}

	std::vector<DwtImage*> getImagesVec4() {
		std::vector<DwtImage*> vectorImages;

		std::vector<DwtImage*> LL;
		std::vector<DwtImage*> HL;
		std::vector<DwtImage*> LH;
		std::vector<DwtImage*> HH;

		
		for(int ch = 0; ch < maxChannel; ch++) {
			for(int l = maxLevel - 1; l > -1; l--) {
				try {
					std::vector<DwtImage*> temp = getDwtImagesByLevelAndChannel(l, ch).createBands();
					for(int i = 0; i < temp.size(); i++) {
						if(temp[i]->getLevel() < maxLevel - 1 && temp[i]->getTypeSubBands() == Bands::LL) {
							continue;
							}

						switch(temp[i]->getTypeSubBands()) {
							case Bands::LL:
								LL.push_back(temp[i]);
								break;
							case Bands::HL:
								HL.push_back(temp[i]);
								break;
							case Bands::LH:
								LH.push_back(temp[i]);
								break;
							case Bands::HH:
								HH.push_back(temp[i]);
								break;
							default:
								break;
							}
						}

					}
				catch(const std::exception&) {
					std::cout << "<-level: " << l << " channel: " << ch << std::endl;
					}

				}
			for(int i = 0; i < LL.size(); i++) {
				vectorImages.push_back(LL[i]);
				}
			LL.clear();

			for(int i = 0; i < HL.size(); i++) {
				vectorImages.push_back(HL[i]);
				}
			HL.clear();

			for(int i = 0; i < LH.size(); i++) {
				vectorImages.push_back(LH[i]);
				}
			LH.clear();

			for(int i = 0; i < HH.size(); i++) {
				vectorImages.push_back(HH[i]);
				}
			HH.clear();

		}
		return vectorImages;
	}

	std::vector<DwtImage*> getImagesVec() {
		std::vector<DwtImage*> vectorImages;

		std::vector<DwtImage*> LL;
		std::vector<DwtImage*> HL;
		std::vector<DwtImage*> LH;
		std::vector<DwtImage*> HH;

		for (int l = maxLevel - 1; l > -1; l--) {
			for (int ch = 0; ch < maxChannel; ch++) {
				try {
					std::vector<DwtImage*> temp = getDwtImagesByLevelAndChannel(l, ch).createBands();
					for (int i = 0; i < temp.size(); i++) {
						if (temp[i]->getLevel() < maxLevel - 1 && temp[i]->getTypeSubBands() == Bands::LL) {
							continue;
						}

						switch (temp[i]->getTypeSubBands()) {
						case Bands::LL:
							LL.push_back(temp[i]);
							break;
						case Bands::HL:
							HL.push_back(temp[i]);
							break;
						case Bands::LH:
							LH.push_back(temp[i]);
							break;
						case Bands::HH:
							HH.push_back(temp[i]);
							break;
						default:
							break;
						}
					}

				}
				catch (const std::exception&) {
					std::cout << "<-level: " << l << " channel: " << ch << std::endl;
				}

			}
			for (int i = 0; i < LL.size(); i++) {
				vectorImages.push_back(LL[i]);
			}
			LL.clear();

			for (int i = 0; i < HL.size(); i++) {
				vectorImages.push_back(HL[i]);
			}
			HL.clear();

			for (int i = 0; i < LH.size(); i++) {
				vectorImages.push_back(LH[i]);
			}
			LH.clear();

			for (int i = 0; i < HH.size(); i++) {
				vectorImages.push_back(HH[i]);
			}
			HH.clear();

		}
		std::vector<DwtImage*> snad;
		if(q){
		
		for(int i = 0; i < vectorImages.size(); i++) {
			DwtImage* dwt = vectorImages[i];
			cv::Mat edit = quant(dwt->getImage(), dwt->getLevel(), dwt->getMaxLevel());
			snad.push_back(new DwtImage(edit, dwt->getLevel(),dwt->getChannel(),dwt->getMaxLevel(), dwt->getMaxChannel(),dwt->getTypeSubBands()) );
		}
		}else{
			snad = 	vectorImages;	
		}

		return snad;
	}

};
#endif 
