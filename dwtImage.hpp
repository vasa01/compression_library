/**********************************************************************************************//**
 * @file	dwtImage.hpp
 *
 * @brief	Declares the dwt image class.
 **************************************************************************************************/

#ifndef DWT_IMAGE
#define DWT_IMAGE

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

enum class Bands{ EMPTY = 0, LL = 1, HL = 2, LH = 3, HH = 4, HLLHHH = 5 };

//using namespace cv;
//using namespace std;

/**********************************************************************************************//**
 * @class	DwtImage
 *
 * @brief	A dwt image.
 *
 * @author	Vasa 0
 * @date	06.01.2017
 **************************************************************************************************/

class DwtImage {
private:
	cv::Mat img;
	int level;
	int channel;
	int maxLevels;
	int maxChannels;
    int width;
    int height;
	Bands type;
	bool isSubBandsCreate;
	bool isEmpty;
	cv::Mat matLL;
	cv::Mat matHL;
	cv::Mat matLH;
	cv::Mat matHH;
public:
	DwtImage():type(Bands::EMPTY), isSubBandsCreate(false), isEmpty(true){

	}

	DwtImage(cv::Mat img, int level,int channel,int maxLevels,int maxChannels):
        img(img), level(level), channel(channel), maxLevels(maxLevels), maxChannels(maxChannels), width(img.cols), height(img.rows), 
        type(Bands::EMPTY), isSubBandsCreate(false), isEmpty(false) {
    }

	DwtImage(cv::Mat img, int level, int channel, int maxLevels, const int maxChannels, Bands type):
        img(img), level(level), channel(channel), maxLevels(maxLevels), maxChannels(maxChannels), width(img.cols), height(img.rows),
        type(type), isSubBandsCreate(false), isEmpty(false) {
	}

    int w() {
        return width;
    }

    int h(){
        return height;
    }

	/**********************************************************************************************//**
	 * @fn	vector<DwtImage*> DwtImage::createBands()
	 *
	 * @brief	Creates the bands.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	Null if it fails, else the new bands.
	 **************************************************************************************************/

	std::vector<DwtImage*> createBands() {
		assert(!img.empty());
		std::vector<DwtImage*> sub;
		matLL = cv::Mat(img, cv::Rect(0, 0, img.cols / 2, img.rows / 2));
		sub.push_back(new DwtImage(matLL, level, channel, maxLevels,maxChannels, Bands::LL));
		matHL = cv::Mat(img, cv::Rect(img.cols / 2, 0, img.cols/2, img.rows / 2));
		sub.push_back(new DwtImage(matHL, level, channel, maxLevels, maxChannels, Bands::HL));
		matLH = cv::Mat(img, cv::Rect(0, img.rows / 2, img.cols/2, img.rows/2));
		sub.push_back(new DwtImage(matLH, level, channel, maxLevels, maxChannels, Bands::LH));
		matHH = cv::Mat(img, cv::Rect(img.cols / 2, img.rows / 2, img.cols/2, img.rows/2));
		sub.push_back(new DwtImage(matHH, level, channel, maxLevels, maxChannels, Bands::HH));
		isSubBandsCreate = true;
		return sub;
	}

	/**********************************************************************************************//**
	 * @fn	int DwtImage::getLevel()
	 *
	 * @brief	Gets the level.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The level.
	 **************************************************************************************************/
	int getMaxLevel() {
		assert(!isEmpty);
		return maxLevels;
	}

	int getLevel() {
		assert(!isEmpty);
		return level;
	}

	/**********************************************************************************************//**
	 * @fn	int DwtImage::getChannel()
	 *
	 * @brief	Gets the channel.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The channel.
	 **************************************************************************************************/
	int getMaxChannel() {
		assert(!isEmpty);
		return maxChannels;
	}


	int getChannel() {
		assert(!isEmpty);
		return channel;
	}

	/**********************************************************************************************//**
	 * @fn	Mat DwtImage::getImage()
	 *
	 * @brief	Gets the image.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The image.
	 **************************************************************************************************/

	cv::Mat getImage() {
		assert(!img.empty());
		return img;
	}

	/**********************************************************************************************//**
	 * @fn	string DwtImage::getTypeSub()
	 *
	 * @brief	Gets type sub.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The type sub.
	 **************************************************************************************************/

	std::string getTypeSub() {
		switch (type) {
		case Bands::EMPTY:
				return "EMPTY";
			case Bands::LL:
				return "LL";
			case Bands::LH:
				return "LH";
			case Bands::HL:
				return "HL";
			case Bands::HH:
				return "HH";
			default:
				return "--";
		}
	}

	/**********************************************************************************************//**
	 * @fn	Bands DwtImage::getTypeSubBands()
	 *
	 * @brief	Gets type sub bands.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @return	The type sub bands.
	 **************************************************************************************************/

	Bands getTypeSubBands() {
		return type;
	}

	void setImage(cv::Mat mat) {
		this->img = mat.clone();
	}
};
#endif 
