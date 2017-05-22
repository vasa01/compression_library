/**********************************************************************************************//**
* @file	cdf53.cpp
*
* @brief	Implements the cdf 5/3 class.
**************************************************************************************************/

#include "cdf53.hpp"


#define WAIT_DELAY 30

#define DWT_VISUAL 0

CDF53::CDF53(int level, cv::Mat& image, int block_size) {
	this->levels = level;
	this->image = image;
	this->block_size = block_size;
	this->countChannels = image.channels();
	}


DwtLevels* CDF53::getLevelsObject() {
	return dwtLevels;
	}


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


void CDF53::forward2d(cv::Mat& image) {
	//const int c = (int)image.channels();	
	this->dwtLevels = new DwtLevels(levels, countChannels, block_size);
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


//void getMinMax(cv::Mat src, float &min, float &max) {
//	assert(!src.empty());
//	cv::Mat src2;
//
//	src.convertTo(src2, CV_32FC1);
//	src2 /= 256;
//	min = 1000;
//	max = 0;
//	//cout << "seem" << endl;
//	for(int y = 0; y < src.rows; y++) {
//		for(int x = 0; x < src.cols; x++) {
//			/*cout << "seem" << endl;
//			cout << x << endl;
//			cout << y << endl;*/
//			if(src2.at<float>(y, x) < min) {
//				min = src2.at<float>(y, x);
//				}
//			if(src2.at<float>(y, x) > max) {
//				max = src2.at<float>(y, x);
//				}
//
//			}
//		}
//	}


double CDF53::test() {
	float min, max;
	cv::Mat orgImage2 = image.clone();
	cv::Mat orgImage = image.clone();
	//orgImage.convertTo(orgImage, CV_32FC3);
	cout << image.cols << endl;
	forward2d(image);
	imwrite("./forward.bmp", image);
	if(0) { //DWT_VISUAL
		vector<DwtImage> vectorLevel = getLevels();
		cout << vectorLevel.size() << endl;
		for(int i = 0; i < vectorLevel.size(); i++) {
			vector<DwtImage*> vectorSubbands = vectorLevel.at(i).createBands();

			for(int a = 0; a < vectorSubbands.size(); a++) {
				if(DWT_VISUAL) {
					imshow("Level_" + Tool::IntToString(vectorSubbands.at(a)->getLevel()) +
						   " Type_" + vectorSubbands.at(a)->getTypeSub() +
						   " Channel_" + Tool::IntToString(vectorSubbands.at(a)->getChannel()), vectorSubbands.at(a)->getImage());
					}
				/*if (true) {
				getMinMax(vectorSubbands.at(0)->getImage(), min, max);
				cout << max;
				cout << "max";
				//cout << max << endl;
				}*/
				}
			/*if (i == 0) {
			cout << vectorSubbands.at(2).getImage() << endl;
			}*/

			}
		cv::waitKey();
		}



	inverse2d(image);
	imwrite("./invers.bmp", image);
	cv::Mat inverse = cv::imread("./invers.bmp", CV_LOAD_IMAGE_COLOR);
	cout << "diff image: " << Tool::Diff(inverse, orgImage2) << endl;
	return 0;//Tool::Diff(image,orgImage);
	}


