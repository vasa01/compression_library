#ifndef MATRIX_H
#define MATRIX_H

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

//#include "tool.hpp"

using namespace std;
//using namespace cv;

class Matrix {
private:
	int width;
	int height;
    vector<int32_t> iMat;
	string decToBin(int32_t number)
	{
		if(number == 0) return "0";
		if(number == 1) return "1";

		if(number % 2 == 0)
			return decToBin(number / 2) + "0";
		else
			return decToBin(number / 2) + "1";
	}

public:
	Matrix() {

	}
	Matrix(cv::Mat mat) {
		this->height = mat.rows;
		this->width = mat.cols;
		this->iMat = vector<int32_t>(height*width, 0);

		for(int y = 0; y < mat.rows; y++) {
			for(int x = 0; x < mat.cols; x++) {
				this->iMat[x * height + y] = static_cast<int32_t>(mat.at<float>(y, x));
			}
		}

	};

	Matrix(cv::Mat mat, float quantization_norm) {
		this->height = mat.rows;
		this->width = mat.cols;
		this->iMat = vector<int32_t>(height*width, 0); 

		for(int y = 0; y < mat.rows; y++) {
			for(int x = 0; x < mat.cols; x++) {
				this->iMat[x * height + y] = static_cast<int32_t>(mat.at<float>(y, x)*quantization_norm);
			}
		}

	};


	Matrix(int **mat, int width, int height) {
		this->height = height;
		this->width = width;

		this->iMat = vector<int32_t>(height*width, 0); 

		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				this->iMat[x * height + y] = mat[x][y];
			}
		}
	};
	//zeros
	Matrix(int width, int height) {
		this->height = height;
		this->width = width;

		this->iMat = vector<int32_t>(height*width, 0); 

		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				this->iMat[x * height + y] = 0;
				}
			}
		};

	/*Matrix::~Matrix() {
		cout << "running destructor Matrix" << endl;
		if (iMat != nullptr) {
			delete[] iMat;
		}
		cout << "running destructor Matrix>>>>>>>>>>>>>>>" << endl;
	};*/

    int getMax() {
        int32_t max = 0;
        for(int x = 0; x < w(); x++) {
            for(int y = 0; y < h(); y++) {
                if(get(y, x) > max) {
                    max = get(y, x);
                    }
                }
            }
        return max;
    }

	int32_t getCountBits() {
		int32_t count=0;
		for(int i = 0; i < iMat.size(); i++) {
			count += decToBin(iMat[i]).length();
		}
		return count;
	}


	int32_t get(cv::Point point) {
		if(point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point !?!" << endl;
			exit(1);
		}
		return this->iMat[point.x * height + point.y];// [point.x][point.y];
	}

    int32_t get(int y, int x) {
		if(y > height - 1 || x > width - 1) {
			cout << "chyba get y:" << y<<" x:"<<x << endl;
			exit(1);
		}
		return this->iMat[x * height + y];// [x][y];
	}

	void set(int y, int x, int32_t value) {
		this->iMat[x * height + y] = value;
	}

	void set(cv::Point point, int32_t value) {
		this->iMat[point.x * height + point.y] = value;
	}

	int& operator[](cv::Point point) {
		if (point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point" << endl;
			exit(1);
		}
		return iMat[point.x * height + point.y];
	}

	int w() {
		return this->width;
	}

	int h() {
		return this->height;
	}

	cv::Mat getCvMat() {
		cv::Mat result = cv::Mat::zeros(height, width, CV_32F);
		for(int x = 0; x < width; x++) {
			for(int y = 0; y < height; y++) {
				result.at<float>(y, x) = get(y,x);
			}
		}
		return result;
	}

	cv::Mat getCvMatDequant(int quantization_norm) {
		cv::Mat result = cv::Mat::zeros(height, width, CV_32F);
		for(int x = 0; x < width; x++) {
			for(int y = 0; y < height; y++) {
				result.at<float>(y, x) = get(y, x);
			}
		}
		result /= quantization_norm;
		return result;
	}
};

class MatrixOfInt{
private:
	int width;
	int height;
	vector<int> iMat;

public:
	MatrixOfInt(int width, int height) {
		this->height = height;
		this->width = width;

		this->iMat = vector<int>(height*width);

	};
	MatrixOfInt() {

	};
	int& operator[](cv::Point point) {
		if (point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point" << endl;
			exit(1);
		}
		return iMat[point.x * height + point.y];
	}


	int get(cv::Point point) {
		if (point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point" << endl;
			exit(1);
		}
		return this->iMat[point.x * height + point.y];// [point.x][point.y];
	}

	int get(int y, int x) {
		if (y > height - 1 || x > width - 1) {
			cout << "chyba get y:" << y << " x:" << x << endl;
			exit(1);
		}
		return this->iMat[x * height + y];// [x][y];
	}

	void set(int y, int x, int value) {
		this->iMat[x * height + y] = value;
	}

	void zeros() {
		for (int i = 0; i < iMat.size(); i++) {
			iMat[i] = 0;
		}
	}

	int w() {
		return this->width;
	}

	int h() {
		return this->height;
	}

};


class MatrixOfString {
private:
	int width;
	int height;
	vector<string> iMat;

public:
	MatrixOfString(int width, int height) {
		this->height = height;
		this->width = width;

		this->iMat = vector<string>(height*width);

	};
	MatrixOfString() {

	};
	string& operator[](cv::Point point){
		if (point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point" << endl;
			exit(1);
		}
		return iMat[point.x * height + point.y];
	}


	string get(cv::Point point) {
		if (point.y > height - 1 || point.x > width - 1) {
			cout << "chyba get point" << endl;
			exit(1);
		}
		return this->iMat[point.x * height + point.y];// [point.x][point.y];
	}

	string get(int y, int x) {
		if (y > height - 1 || x > width - 1) {
			cout << "chyba get y:" << y << " x:" << x << endl;
			exit(1);
		}
		return this->iMat[x * height + y];// [x][y];
	}

	void set(int y, int x, string value) {
		this->iMat[x * height + y] = value;
	}

	int w() {
		return this->width;
	}

	int h() {
		return this->height;
	}

};

#endif
