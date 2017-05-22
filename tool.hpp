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
 * @file	tool.hpp
 *
 * @brief	tool in library.
 */
#ifndef TOOL_H
#define TOOL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <iterator>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui/highgui.hpp>

#include "matrix.hpp"

#include "fileIOStream.hpp"
#include "bitIOStream.hpp"

#define VEC3DINT8 vector<vector<vector<int8_t>>>
#define MAXBIN15 32768
//using namespace std;
//using namespace cv;


/**
 * struct Element
 */
struct Element {
	enum class Code { Pos, Neg, IsolatedZero, ZeroTreeRoot };

	Element() {}
	Element(int x, int y): x(x), y(y) { }

	int x, y;
	Code code;
};
/**
 * class ChangingField
 */
class ChangingField {
public:
	ChangingField() {

	}

	ChangingField(int size) {
		/*for (int i = 0; i < size; i++) {
			vec.push_back(0);
		}*/
		vec = vector<int>(size, 0);
	}

	int &operator[](int index) {
		if (index > 1000000) {
			cerr << "ChangingField is greater than the predefined maximum value" << endl;
			exit(1);
		}
		if(index > vec.size()-1) {
			while (1) {
				if (index < vec.size()) {
					break;
				}
				else {
					vec.push_back(0);
				}
			}
		}
		return vec[index];
	}

	vector<int> getVector() {
		return vec;
	}
private:
	vector<int> vec;
};


/**********************************************************************************************//**
 * @namespace	Tool
 *
 * @brief	.
 **************************************************************************************************/

namespace Tool {
	
/**
 * sign
 * @tparam T
 * @param val
 * @return
 */
	template <typename T>
	static bool Sign(T val) {
		return val >= 0 ? true : false;
	}
	
/**
 * Diff cv::Mat
 * @param test
 * @param ref
 * @return
 */

	static double Diff(const cv::Mat& test, const cv::Mat& ref) {
		cv::Mat diff;
		absdiff(test, ref, diff);
		return sum(diff).val[0] / (diff.rows*diff.cols);
	}

/**
 * DecToBin
 * @tparam T
 * @param number
 * @return
 */
	template <typename T>
	static string DecToBin(T number)
	{
		if (number == 0) return "0";
		if (number == 1) return "1";

		if (number % 2 == 0)
			return DecToBin(number / 2) + "0";
		else
			return DecToBin(number / 2) + "1";
	}
/**
 * BitAnd
 * @tparam T
 * @param a
 * @param b
 * @return
 */
	template <typename T>
	static T BitAnd(T a, T b) {
		return a & b;
	}

/**
 * BitAnd
 * @param a
 * @param b
 * @return
 */
	static uint32_t BitAnd(uint32_t a, uint32_t b) {
		return a & b;
	}


/**
 * BitAnd
 * @tparam T
 * @param a
 * @return
 */
	template <typename T>
	static T BitCmp(T a) {
		return ~a;
	}


/**
 * BitCmp
 * @param a
 * @return
 */
	static uint32_t BitCmp(uint32_t a) {
		return ~a;
	}


/**
 * SetBitFromInt
 * @param a
 * @param p
 * @param value
 * @param test
 * @return
 */
	static uint32_t SetBitFromInt(uint32_t a, int p, int value, int test) {
		uint32_t result = 0;
		vector <int> bits;
		/*string ted = Tool::DecToBin(a);
		for(int i = 0; i < 10; i++) {
			ted.push_back('y');
		}
		
		ted[p] = 'x';
		if(test == 10) {
			cout <<  ted << endl;
		}*/
		// continously divide the integer by 2, if there is no remainder, the bit is 1, else it's 0
		for (int i = 0; i < 32; i++)
		{
			if (i != p-1) {
				bits.push_back(a % 2);    // remainder of dividing by 2
			}
			else {
				bits.push_back(value); 
			}
			a /= 2;    // integer equals itself divided by 2
		}
		int factor=1;
		for (int i = 0; i < bits.size(); i++) {
			result += bits[i]*factor;
			/*if(test == 10) {
				cout << temp << endl;
			}*/
			factor *= 2;
		}
		return result;
	}

/**
 * GetBitFromInt
 * @param a
 * @param p
 * @return
 */
	static int GetBitFromInt(uint32_t a, int p) {
		vector <int> bits;

		// continously divide the integer by 2, if there is no remainder, the bit is 1, else it's 0
		for (int i = 0; i < 32; i++)
		{
			bits.push_back(a % 2);    // remainder of dividing by 2
			a /= 2;    // integer equals itself divided by 2
		}

		return bits[p-1];
	}
/**
 * GetPower
 * @param value
 * @param exponent
 * @return
 */
	static uint32_t GetPower(int value, int exponent) {
		uint32_t result = 1;
		for(int i = 0; i < exponent; i++) {
			result *= value;
		}
		return result;
	}

/**
 * BitShift
 * @tparam T
 * @param a
 * @param b
 * @return
 */
	template <typename T>
	static T BitShift(T a, T b) {
		/*if (b < 0) {
			return a >> b;
		}
		else {*/
			return a << b;
		//}
		
	}
/**
 * BitShift
 * @param a
 * @param b
 * @return
 */

	static uint32_t BitShift(uint32_t a, int b) {
		if (b < 0) {
			b *= -1;
		return a >> b;
		}
		else {
		return a << b;
		}

	}



/**
 * GetMaxInArray
 * @param array
 * @param size
 * @return
 */
	static int GetMaxInArray(int* array, int size) {
		int max = 0;
		for (int i = 0; i < size; i++) {
			if (array[i] > max) {
				max = array[i];
			}
		}
		return max;
	}

/**
 * GetMinInArray
 * @param array
 * @param size
 * @return
 */
	static int GetMinInArray(int* array, int size) {
		int min = 1000000;
		for (int i = 0; i < size; i++) {
			if (array[i] < min) {
				min = array[i];
			}
		}
		return min;
	}


/**
 * Int to string
 * @param a
 * @return
 */
	static string IntToString(int a)
	{
		ostringstream temp;
		temp << a;
		return temp.str();
	}

/**
 * Rect subarea in Matrix
 * @param mat
 * @param x
 * @param y
 * @param width
 * @param height
 * @return - subarea
 */
	static Matrix Rect(Matrix mat, int x, int y, int width, int height) {
		//int **newMat = Tool::AlllocMatrixOfInt(height, width);
		//cout << " width " << width << " height " << height << endl;
		Matrix newMat = Matrix(width, height);
		int xx = 0, yy = 0;
		for (int newY = y; newY < height + y; newY++) {
			for (int newX = x; newX < width + x; newX++) {
				//newMat[xx][yy] = mat.get(newY, newX);
				newMat.set(yy, xx, mat.get(newY, newX));
				xx++;
				//cout << "new " << newX - x << " " << newY - y << " old " << endl;
			}
			xx = 0;
			yy++;
		}
		//cout << "konec" << endl;
		return newMat;
	}

    /**
     * Abs
     * @param mat - Matrix
     * @return abs matrix
     */
	static Matrix Abs(Matrix mat) {
		//int **newMat = Tool::AlllocMatrixOfInt(mat.h(), mat.w());
		Matrix newMat = Matrix(mat.w(), mat.h());
		for (int y = 0; y < mat.h(); y++) {
			for (int x = 0; x < mat.w(); x++) {
				if (mat.get(y, x) < 0) {
					//newMat[x][y] = mat.get(y, x)*(-1);
					newMat.set(y, x, mat.get(y, x)*(-1));
				}
				else {
					//newMat[x][y] = mat.get(y, x);
					newMat.set(y, x, mat.get(y, x));
				}
			}
		}
		return newMat;
	}


    /**
     * DealllocMatrixOfInt
     * @param array
     * @param width
     */
	static void DealllocMatrixOfInt(int** array, int width) {
		for (int i = 0; i < width; ++i) {
			delete[] array[i];
		}
		delete[] array;
	}


    /**
     * GetMaxInMat
     * @param src
     * @return  - max in cv::Mat
     */
	static int GetMaxInMat(cv::Mat src) {
		assert(!src.empty());
		cv::Mat src2;
		int max = 0;
		for (int x = 0; x < src.cols; x++) {
			for (int y = 0; y < src.rows; y++) {
				if (src.at<float>(y, x) > max) {
					max = (int)src.at<float>(y, x);
				}

			}
		}
		return max;
	}

    /**
     * GetMaxInMat
     * @param src
     * @return  - max value in matrix
     */
	static int GetMaxInMat(Matrix src) {
		//assert(!src.empty());
		//Mat src2;
        int32_t max = 0;
		for(int x = 0; x < src.w(); x++) {
			for(int y = 0; y < src.h(); y++) {
				if(src.get(y, x) > max) {
					max = src.get(y, x);
				}
			}
		}
		return max;
	}







	static VEC3DINT8 Zeros(int height, int width, int depth) {
		VEC3DINT8 array3D;

		array3D.resize(height);
		for (int i = 0; i < height; ++i) {
			array3D[i].resize(width);

			for (int j = 0; j < width; ++j)
				array3D[i][j].resize(depth);
		}

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				for (int d = 0; d < depth; d++) {
					array3D[y][x][d] = 0;
				}
			}
		}

		return array3D;
	}


/**
 * BitXor
 * @tparam T
 * @param x
 * @param y
 * @return
 */
	template <typename T>
	static T BitXor(T x, T y)
	{
		return x ^ y;
	}


/**
 * StringBinToDec
 * @param bin
 * @return - dec number
 */
	static int32_t StringBinToDec(string bin) {
		int32_t factor = 1;
		int len = (int)bin.length()-1;
		int32_t result = 0;
		while (len > -1) {
			result += (bin[len] - '0') * factor;
			factor *= 2;
			len -= 1;
		}
		
		return result;
	}
/**
 * Pop front in vector
 * @tparam T
 * @param vec
 */
	template<typename T>static void PopFront(std::vector<T>& vec){
		assert(!vec.empty());
		vec.erase(vec.begin());
	}
/**
 * GetMaxInDeque
 * @tparam T
 * @param vec - deque
 * @return - max in deque
 */
	template<typename T>static T GetMaxInDeque(deque<T>& vec) {
		T max = 0;
		for (int i = 0; i < vec.size(); i++) {
			//cout << vec[i] << endl;
			if (vec[i] > max) {
				max = vec[i];
			}
		}
		return max;
	}
/**
 * GetMaxInVector
 * @tparam T
 * @param vec  - vector
 * @return  - max value in vec
 */
	template<typename T>static T GetMaxInVector(vector<T>& vec) {
		T max = 0;
		for (int i = 0; i < vec.size(); i++) {
			//cout << vec[i] << endl;
			if (vec[i] > max) {
				max = vec[i];
			}
		}
		return max;
	}


    /**
     * SplitImage to vector
     * @param input - image
     * @param size - sub blok
     * @return - vector mat
     */
	static vector<cv::Mat> SplitImage(cv::Mat input, int size) {
		vector<cv::Mat> vectorImages2;


		if(input.cols > size || input.rows > size) {
			for(int y = 0; y < input.rows; y += size) {
				for(int x = 0; x < input.cols; x += size) {
					int tempWidth = size;
					int tempHeight = size;
					if(y + tempHeight > input.rows) {
						tempHeight = input.rows - y;
						}
					if(x + tempWidth > input.cols) {
						tempWidth = input.cols - x;
						}
					cv::Rect rect = cv::Rect(x, y, tempWidth, tempHeight);
					vectorImages2.push_back(cv::Mat(input, rect));
				}
			}
		}
		else {
			vectorImages2.push_back(input);
		}

		return vectorImages2;
	}

    /**
     * JoinImage
     * @param input - vector cv:Mat
     * @param width - new image width
     * @param height - new image height
     * @return - new mat
     */
	static cv::Mat JoinImage(vector<cv::Mat> input, int width, int height) {
		cv::Mat output = cv::Mat(height, width, input[0].type());
		int newY =0, newX=0;
		for(int i = 0; i < input.size(); i++) {
			cv::Rect rect = cv::Rect(newX, newY, input[i].cols, input[i].rows);
			input[i].copyTo(output(rect));
			newX += input[i].cols;
			if(newX == width) {
				newX = 0;
				newY += input[i].rows;
			}
		}
		return output;
	}

    /**
     *
     * @param b
     * @return
     */
	static uint8_t PopCount(uint8_t b) {

		b = ((b & 0xAA) >> 1) + (b & 0x55);
		b = ((b & 0xCC) >> 2) + (b & 0x33);
		b = ((b & 0xF0) >> 4) + (b & 0x0F);

		return b;
		}

    /**
     *
     * @param b
     * @return
     */
	static uint8_t PopCount(uint16_t b) {

		b = ((b & 0xAAAA) >> 1) + (b & 0x5555);
		b = ((b & 0xCCCC) >> 2) + (b & 0x3333);
		b = ((b & 0xF0F0) >> 4) + (b & 0x0F0F);
		b = ((b & 0xFF00) >> 8) + (b & 0x00FF);

		return b;
		}

    /**
     *
     * @param b
     * @return
     */
	static uint8_t PopCount(uint32_t b) {

		b = ((b & 0xAAAAAAAA) >> 1) + (b & 0x55555555);
		b = ((b & 0xCCCCCCCC) >> 2) + (b & 0x33333333);
		b = ((b & 0xF0F0F0F0) >> 4) + (b & 0x0F0F0F0F);
		b = ((b & 0xFF00FF00) >> 8) + (b & 0x00FF00FF);
		b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);

		return b;
		}


    /**
     * SplitVector
     * @tparam T
     * @param vec
     * @param n
     * @return
     */
	template<typename T>
	static std::vector<std::vector<T>> SplitVector(const std::vector<T>& vec, size_t n)
		{
		std::vector<std::vector<T>> outVec;

		for(int i = 0; i < n; i++) {
			outVec.push_back(vector<T>());
			}
		//int counter=0;
		for(int i = 0; i < vec.size(); i = i + n) {
			int counter = i;
			for(int ii = 0; ii < n; ii++) {
				outVec[ii].push_back(vec[counter]);
				counter++;
				}
			}

		return outVec;
	}

    /**
     * GetQuantizationNorm
     * @param in
     * @return
     */
	static float GetQuantizationNorm(int in){
		float result = 1;
		float factor = 1;
		int counter = std::abs(in);
		if(in < 0){
			factor = 0.1;
		}else if(in > 0){
			factor = 10;
		}
		for(int i=0; i < counter; i++){
			result *= factor;
		}

		return result;
	}

};
#endif 


