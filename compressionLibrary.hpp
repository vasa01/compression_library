#ifndef COMPRESSION_LIBRARY_H
#define COMPRESSION_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui/highgui.hpp>

//#include "colorSpace.hpp"

//#include "source.cpp"
//#include "dlib/compress_stream.h"

#include "cdf97.hpp"
#include "cdf53.hpp"

////#include "encodeBitPlaneCoder.hpp"
////#include "mq_encode.hpp"
////#include "mq_decode.hpp"
////#include "decodeBitPlaneCoder.hpp"

#include "EZWDecode.hpp"
#include "EZWEncode.hpp"

#include "spihtEncode.hpp"
#include "spihtDecode.hpp"

//#include "compressBitStream.hpp"
#include "fileIOStream.hpp"


#include "matrix.hpp"

#include "tool.hpp"
#include "joinSubimage.hpp"

#include "ariEncode.hpp"
#include "ariDecode.hpp"
//#include "balz.hpp"

#include <queue>

#include "mat.hpp"
#include "contourlet.hpp"
#include "load_mat.hpp"
#include "save_mat.hpp"
#include "mycountourlet.hpp"

#include "mq_new.hpp"

#include "block_encoder.hpp"
#include "block_decoder.hpp"

#include "mq_encoder2.hpp"


//using namespace std;
//using namespace cv;


//namespace Tool {
//	enum TRANS_TYPE { DWT, CON };
//	enum CODE_TYPE { EZW, SPIHT, EBCOT };
//	enum LEVEL_NUM {LEVEL_0=0, LEVEL_1=1, LEVEL_2=2, LEVEL_3=3, LEVEL_4=4, LEVEL_5 = 5, LEVEL_6 = 6,  LEVEL_7 = 7};
//};

#define YR (0.2567890625) // (65,738/256)
#define YG (0.50412890625) // (129,057/256)
#define YB (0.09790625) // (25,064/256)

#define CbR (0.14822265625) // (37,945/256)
#define CbG (0.2909921875) // (74,494/256)
#define CbB (0.43921484375) // (112,439/256)

#define CrR (0.43921484375) // (112,439/256)
#define CrG (0.3677890625) // (94,154/256)
#define CrB (0.07142578125) // (18,285/256)


struct EBCOT_info {
	EBCOT_info(int _width, int _height, int _level, int _channel, int _band, int _plane, int _min, vector<uint8_t> _buffer):width(_width), height(_height),
	level(_level), channel(_channel), band(_band), plane(_plane), min(_min), buffer_size(_buffer.size()), buffer(_buffer) {  }
	int width;
	int height;
	int level;
	int channel;
	int band;
	int plane;
	int min;
	int buffer_size;
	vector<uint8_t> buffer;
};

namespace {
	double RGBY = (1.1643835616438356164383561643836);// (255/219)

	double RCr = (1.5960267857142857142857142857143); // ((255/112)*0,701)

	double GCb = (0.39176229009491360428328060355318); // (100,291/256)
	double GCr = (0.81296764723777074714042346069603); // (208,120/256)

	double BCb = (2.0172321428571428571428571428571);// (516,412/256)
}


namespace  COMPR_LBR {
	enum TRANS_TYPE { DWT_CDF97, DWT_CDF53, CON };
	enum CODE_TYPE { EZW, SPIHT, EBCOT };
	enum LEVEL_NUM { LEVEL_0 = 0, LEVEL_1 = 1, LEVEL_2 = 2, LEVEL_3 = 3, LEVEL_4 = 4, LEVEL_5 = 5, LEVEL_6 = 6, LEVEL_7 = 7, LEVEL_8 = 8, LEVEL_9 = 9, LEVEL_10 = 10, LEVEL_11 = 11};

	enum COLOR_SPACE_TYPE { GRAY = 2, BGR = 1, YCBCR = 0 };


};


static cv::Mat ConvertBRG2GRAY(cv::Mat src) {
	cv::Mat result;
	cvtColor(src, result, CV_BGR2GRAY);
	return result;
};

static cv::Mat ConvertBRG2YCbCr(cv::Mat src) {
	//Mat output(src.size(), CV_32FC3);
	double min, max;
	minMaxLoc(src, &min, &max);
	//cout << "BGR min: " << min << " max: " << max << endl;
	src.convertTo(src, CV_32FC3);
	minMaxLoc(src, &min, &max);
	//cout << "BGR min: " << min << " max: " << max << endl;
	cv::Mat output = src.clone();
	//iterate over all pixels
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			//get bgr pixel
			//cout << "for" << endl;
			cv::Vec3f bgrPixel = src.at<cv::Vec3f>(i, j);
			//cout << "for" << endl;
			float B = bgrPixel[0];
			float G = bgrPixel[1];
			float R = bgrPixel[2];

			//actual conversion from BGR to YCrCb
			//float delta = 0.5f;

			/*float Y = 0.257*R + 0.504*G + 0.098*B + 16;
			float Cb = -0.148*R - 0.291*G + 0.439*B + 128;
			float Cr = 0.439*R - 0.368*G - 0.071*B + 128;*/

			float Y = YR*R + YG*G + YB*B + 16;
			float Cb = -CbR*R - CbG*G + CbB*B + 128;
			float Cr = CrR*R - CrG*G - CrB*B + 128;

			/*float Y = 0.299*R + 0.587*G + 0.114*B;
			float Cb = (B-Y)*0.564 + delta;
			float Cr = (R-Y)*0.713 + delta;*/

			//store into result image
			cv::Vec3f yCrCbPixel(Y, Cb, Cr);
			output.at<cv::Vec3f>(i, j) = yCrCbPixel;
		}
	}
	minMaxLoc(output, &min, &max);
	//cout << "BGR min: " << min << " max: " << max << endl;
	//output.convertTo(output, CV_8UC1);
	minMaxLoc(output, &min, &max);
	//cout << "BGR min: " << min << " max: " << max << endl;
	return output;
};

static cv::Mat ConvertYCbCr2BGR(cv::Mat src) {
	double min, max;
	minMaxLoc(src, &min, &max);
	//cout << "YCbCr min: " << min << " max: " << max << endl;
	src.convertTo(src, CV_32FC3);
	//Mat output(src.size(), CV_32FC3);
	minMaxLoc(src, &min, &max);
	//cout << "YCbCr min: " << min << " max: " << max << endl;
	cv::Mat output = src.clone();
	//iterate over all pixels
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			//get bgr pixel 
			cv::Vec3f YCbCrPixel = src.at<cv::Vec3f>(i, j);

			float Y = YCbCrPixel[0];
			float Cb = YCbCrPixel[1];
			float Cr = YCbCrPixel[2];

			//actual conversion from  YCrCb to BGR 
			//float delta = 0.5f;

			/*float B = 1.164*(Y - 16) + 2.017*(Cb - 128);
			float G = 1.164*(Y - 16) - 0.813*(Cr - 128) - 0.392*(Cb - 128);
			float R = 1.164*(Y - 16) + 1.596*(Cr - 128);*/

			/*float B = RGBY*Y + BCb*Cb - constB;
			float G = RGBY*Y - GCr*Cr - GCb*Cb + constG;
			float R = RGBY*Y  + RCr*Cr - constR;*/

			float B = RGBY*(Y - 16) + BCb*(Cb - 128);
			float G = RGBY*(Y - 16) - GCr*(Cr - 128) - GCb*(Cb - 128);
			float R = RGBY*(Y - 16) + RCr*(Cr - 128);

			/*float B = Y + 1.773*(Cb-delta);
			float G = Y-0.714*(Cr-delta)-0.344*(Cb-delta);
			float R = Y + 1.403*(Cr-delta);*/

			//store into result image
			if(R < 0) {
				R = 0;
			}
			else if(R > 255) {
				R=255;
			}

			if(G < 0) {
				G = 0;
			}
			else if(G > 255) {
				G = 255;
			}

			if(B < 0) {
				B = 0;
			}
			else if(B > 255) {
				B = 255;
			}
			cv::Vec3f BGRPixel(B, G, R);
			output.at<cv::Vec3f>(i, j) = BGRPixel;
		}
	}
	minMaxLoc(output, &min, &max);
	//cout << "YCbCr min: " << min << " max: " << max << endl;
	//output.convertTo(output, CV_8UC1);
	minMaxLoc(output, &min, &max);
	//cout << "YCbCr min: " << min << " max: " << max << endl;
	return output;
};






cv::Mat decompresFile(string inputSource, string outputSource);
void compresFile(string inputSource, string outputSource, COMPR_LBR::COLOR_SPACE_TYPE codeColorType, COMPR_LBR::TRANS_TYPE transType, COMPR_LBR::LEVEL_NUM levelNum, COMPR_LBR::CODE_TYPE codeType, vector<bool> set, vector<int> setValue, int block_size);

#endif