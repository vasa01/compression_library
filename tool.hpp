/**********************************************************************************************//**
 * @file	tool.hpp
 *
 * @brief	Declares the tool class.
 **************************************************************************************************/

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



struct Element {
	enum class Code { Pos, Neg, IsolatedZero, ZeroTreeRoot };

	Element() {}
	Element(int x, int y): x(x), y(y) { }

	int x, y;
	Code code;
};

struct CodingVectors {
	CodingVectors(vector<int> _CX, vector<int> _D) :CX(_CX), D(_D) {  }
	vector<int> CX, D;
};


class ClassCodingVectors {
public:
	ClassCodingVectors() {

	}

	ClassCodingVectors(CodingVectors cod) {
		vector<int> _cx, _d;
		for (int i = 0; i < cod.CX.size(); i++) {
			if (cod.CX[i] == 20 && cod.D[i] == 2) {
				addVector(CodingVectors(_cx, _d));
				_cx.clear();
				_d.clear();
			}
			else {
				_cx.push_back(cod.CX[i]);
				_d.push_back(cod.D[i]);
			}
		}
		addVector(CodingVectors(_cx, _d));
		//cout << "storage size" <<storage.size() << endl;
	}

	void ClassCodingVectors::addVector(CodingVectors vectors) {
		storage.push_back(vectors);
	}

	void ClassCodingVectors::setP(vector<int> _P) {
		P = _P;
	}
	vector<int> ClassCodingVectors::getP() {
		return P;
	}

	CodingVectors ClassCodingVectors::get(int index) {
		if (index > storage.size() - 1) {
			cerr << "Error Coding vectors with index: " << index << "not esist." << endl;
			exit(1);
		}
		return storage[index];
	}

	CodingVectors ClassCodingVectors::getJoin() {
		//cout << "storage size" << storage.size() << endl;
		vector<int> newCX, newD;
		///
		for (int i = 0; i < storage.size(); i++) {
			for (int s = 0; s < storage[i].CX.size(); s++) {
				newCX.push_back(storage[i].CX[s]);
				newD.push_back(storage[i].D[s]);
			}
			if (i != storage.size() - 1) {
				newCX.push_back(20);
				newD.push_back(2);
			}
		}
		///
		return CodingVectors(newCX, newD);
	}

	int ClassCodingVectors::sizeCodingVectors() {
		return storage.size();
	}

private:
	vector<CodingVectors> storage;
	vector<int> P;


};

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
	
	/*#define BLOCK_SIZE 128
	#define TILE_SIZE 128*/
	
	template <typename T>
	static bool Sign(T val) {
		return val >= 0 ? true : false;
	}
	
	/**********************************************************************************************//**
	 * @fn	double Diff(const Mat& test, const Mat& ref)
	 *
	 * @brief	Compares two const Mat&amp; objects to determine their relative ordering.
	 *
	 * @author	Vasa 0
	 * @date	02.02.2017
	 *
	 * @param	test	The test.
	 * @param	ref 	The reference.
	 *
	 * @return	A double.
	 **************************************************************************************************/

	static double Diff(const cv::Mat& test, const cv::Mat& ref) {
		cv::Mat diff;
		absdiff(test, ref, diff);
		return sum(diff).val[0] / (diff.rows*diff.cols);
	}

	/**********************************************************************************************//**
	 * @fn	static string DecToBin(int number)
	 *
	 * @brief	Decrement to bin.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	number	Number of.
	 *
	 * @return	A string.
	 **************************************************************************************************/
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
	/*
	static string DecToBin(uint32_t number)
		{
		if(number == 0) return "0";
		if(number == 1) return "1";

		if(number % 2 == 0)
			return DecToBin(number / 2) + "0";
		else
			return DecToBin(number / 2) + "1";
		}*/

	/**********************************************************************************************//**
	 * @fn	static uint8_t BitAnd(uint8_t a, uint8_t b)
	 *
	 * @brief	Bit and.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint8_t to process.
	 * @param	b	The uint8_t to process.
	 *
	 * @return	An uint8_t.
	 **************************************************************************************************/
	template <typename T>
	static T BitAnd(T a, T b) {
		return a & b;
	}

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitAnd(uint32_t a, uint32_t b)
	 *
	 * @brief	Bit and.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 * @param	b	The uint32_t to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/
	
	static uint32_t BitAnd(uint32_t a, uint32_t b) {
		return a & b;
	}

	/**********************************************************************************************//**
	 * @fn	static int BitAnd(int a, int b)
	 *
	 * @brief	Bit and.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The int to process.
	 * @param	b	The int to process.
	 *
	 * @return	An int.
	 **************************************************************************************************/
	/*
	static int BitAnd(int a, int b) {
		/*uint8_t result = 0;
		uint8_t coef = 1;
		while (a && b) {
			result += ((a % 10) && (b % 10)) * coef;
			coef *= 10;
			a /= 10;
			b /= 10;
		}
		return a & b;//result;
	}*/

	/**********************************************************************************************//**
	 * @fn	static int BitCmp(int a)
	 *
	 * @brief	Bit compare.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The int to process.
	 *
	 * @return	An int.
	 **************************************************************************************************/
	template <typename T>
	static T BitCmp(int T) {
		return ~a;
	}

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitCmp(uint32_t a)
	 *
	 * @brief	Bit compare.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/
	
	static uint32_t BitCmp(uint32_t a) {
		return ~a;
	}

	/**********************************************************************************************//**
	 * @fn	static uint8_t BitCmp(uint8_t a)
	 *
	 * @brief	Bit compare.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint8_t to process.
	 *
	 * @return	An uint8_t.
	 **************************************************************************************************/

	/*static uint8_t BitCmp(uint8_t a) {
		return ~a;
	}*/

	/**********************************************************************************************//**
	 * @fn	static uint32_t SetBitFromInt(uint32_t a, int p, int value)
	 *
	 * @brief	Sets bit from int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	 	The uint32_t to process.
	 * @param	p	 	The int to process.
	 * @param	value	The value.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/

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

	/**********************************************************************************************//**
	 * @fn	static int GetBitFromInt(uint32_t a, int p)
	 *
	 * @brief	Gets bit from int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 * @param	p	The int to process.
	 *
	 * @return	The bit from int.
	 **************************************************************************************************/

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

	static uint32_t GetPower(int value, int exponent) {
		uint32_t result = 1;
		for(int i = 0; i < exponent; i++) {
			result *= value;
		}
		return result;
	}

	/**********************************************************************************************//**
	 * @fn	static int BitShift(int a, int b)
	 *
	 * @brief	Bit shift.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The int to process.
	 * @param	b	The int to process.
	 *
	 * @return	An int.
	 **************************************************************************************************/
	template <typename T>
	static T BitShift(T a, T b) {
		/*if (b < 0) {
			return a >> b;
		}
		else {*/
			return a << b;
		//}
		
	}

	/**********************************************************************************************//**
	 * @fn	static uint8_t BitShift(uint8_t a, uint8_t b)
	 *
	 * @brief	Bit shift.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint8_t to process.
	 * @param	b	The uint8_t to process.
	 *
	 * @return	An uint8_t.
	 **************************************************************************************************/
	
	//static uint8_t BitShift(uint8_t a, uint8_t b) {
	//	/*if (b < 0) {
	//	return a >> b;
	//	}
	//	else {*/
	//	return a << b;
	//	//}

	//}

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitShift(uint32_t a, uint8_t b)
	 *
	 * @brief	Bit shift.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 * @param	b	The uint8_t to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/

	/*static uint32_t BitShift(uint32_t a, uint8_t b) {
		if (b < 0) {
			b *= -1;
		return a >> b;
		}
		else {
		return a << b;
		}

	}*/

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitShift(uint32_t a, int b)
	 *
	 * @brief	Bit shift.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 * @param	b	The int to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/

	static uint32_t BitShift(uint32_t a, int b) {
		if (b < 0) {
			b *= -1;
		return a >> b;
		}
		else {
		return a << b;
		}

	}

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitShift(uint32_t a, uint32_t b)
	 *
	 * @brief	Bit shift.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The uint32_t to process.
	 * @param	b	The uint32_t to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/

	/*static uint32_t BitShift(uint32_t a, uint32_t b) {
		if (b < 0) {
			b *= -1;
		return a >> b;
		}
		else {
		return a << b;
		}

	}*/

	/**********************************************************************************************//**
	 * @fn	static int GetMaxInArray(int* array, int size)
	 *
	 * @brief	Gets maximum in array.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	array	If non-null, the array.
	 * @param 		  	size 	The size.
	 *
	 * @return	The maximum in array.
	 **************************************************************************************************/

	static int GetMaxInArray(int* array, int size) {
		int max = 0;
		for (int i = 0; i < size; i++) {
			if (array[i] > max) {
				max = array[i];
			}
		}
		return max;
	}

	/**********************************************************************************************//**
	 * @fn	static int GetMinInArray(int* array, int size)
	 *
	 * @brief	Gets minimum in array.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	array	If non-null, the array.
	 * @param 		  	size 	The size.
	 *
	 * @return	The minimum in array.
	 **************************************************************************************************/

	static int GetMinInArray(int* array, int size) {
		int min = 1000000;
		for (int i = 0; i < size; i++) {
			if (array[i] < min) {
				min = array[i];
			}
		}
		return min;
	}

	/**********************************************************************************************//**
	 * @fn	static string IntToString(int a)
	 *
	 * @brief	Int to string.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	a	The int to process.
	 *
	 * @return	A string.
	 **************************************************************************************************/

	static string IntToString(int a)
	{
		ostringstream temp;
		temp << a;
		return temp.str();
	}

	/**********************************************************************************************//**
	 * @fn	static int** AlllocMatrixOfInt(int height, int width)
	 *
	 * @brief	Allloc matrix of int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	height	The height.
	 * @param	width 	The width.
	 *
	 * @return	Null if it fails, else a handle to an int.
	 **************************************************************************************************/

	/*static int** AlllocMatrixOfInt(int height, int width){
		int **result = new int*[width];
		for (int i = 0; i < width; ++i) {
			result[i] = new int[height];
		}
		return result;
	}*/

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

	/**********************************************************************************************//**
	 * @fn	static string** AllocMatrixOfString(int width, int height)
	 *
	 * @brief	Allocate matrix of string.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	width 	The width.
	 * @param	height	The height.
	 *
	 * @return	Null if it fails, else a handle to a string.
	 **************************************************************************************************/

	/*static string** AllocMatrixOfString(int width, int height) {
		string **array = new string*[width];

		for (int i = 0; i < width; ++i) {
			array[i] = new string[height];
		}
		//alloc.push_back(array);
		//alloc_width.push_back(width);
		return array;
	}


	static void DeallocMatrixOfString(string** mat, int width) {
		for (int i = 0; i < width; i++) {
			delete[] mat[i];
		}
		delete[] mat;
	}*/
	/**********************************************************************************************//**
	 * @fn	static void DealllocMatrixOfInt(int** array, int width)
	 *
	 * @brief	Deallloc matrix of int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	array	If non-null, the array.
	 * @param 		  	width	The width.
	 **************************************************************************************************/

	static void DealllocMatrixOfInt(int** array, int width) {
		for (int i = 0; i < width; ++i) {
			delete[] array[i];
		}
		delete[] array;
	}

	/**********************************************************************************************//**
	 * @fn	static int GetMaxIn2dArray(int** array)
	 *
	 * @brief	Gets maximum in 2D array.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	array	If non-null, the array.
	 *
	 * @return	The maximum in 2D array.
	 **************************************************************************************************/

	static int GetMaxIn2dArray(int** array) {
		
	}

	/**********************************************************************************************//**
	 * @fn	static int GetMaxInMat(Mat src)
	 *
	 * @brief	Gets maximum in matrix.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	src	Source for the.
	 *
	 * @return	The maximum in matrix.
	 **************************************************************************************************/

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

	/**********************************************************************************************//**
	 * @fn	static int** FloatMatTo2DArray(Mat mat)
	 *
	 * @brief	Float matrix to 2D array of int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	mat	The matrix.
	 *
	 * @return	Null if it fails, else a handle to an int.
	 **************************************************************************************************/

	/*static  int** FloatMatTo2DArray(Mat mat) {
		int** result = AlllocMatrixOfInt(mat.rows, mat.cols);
		cout << "Mat cols:" << mat.cols << " Mat rows:" << mat.rows << endl;
		for (int x = 0; x < mat.cols; x++) {
			for (int y = 0; y < mat.rows; y++) {
				result[x][y] = (int)(mat.at<float>(y, x) * 1.0);
			}
		}
		return result;
	}*/

	/**********************************************************************************************//**
	 * @fn	static int** UcharMatTo2DArray(Mat mat)
	 *
	 * @brief	Uchar matrix to 2D array.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	mat	The matrix.
	 *
	 * @return	Null if it fails, else a handle to an int.
	 **************************************************************************************************/

	/*static  int** UcharMatTo2DArray(Mat mat) {
		int** result = AlllocMatrixOfInt(mat.rows, mat.cols);
		cout << "Mat cols:" << mat.cols << " Mat rows:" << mat.rows << endl;
		for (int x = 0; x < mat.cols; x++) {
			for (int y = 0; y < mat.rows; y++) {
				result[x][y] = mat.at<uchar>(y, x);
			}
		}
		return result;
	}*/

	/**********************************************************************************************//**
	 * @fn	static int* Zeros(int count)
	 *
	 * @brief	Zeros array.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	count	Number of.
	 *
	 * @return	Null if it fails, else a pointer to an int.
	 **************************************************************************************************/

	/*static int* Zeros(int count) {
		int* result =  new int[count];
		for (int i = 0; i < count; i++) {
			result[i] = 0;
		}
		return result;
	}*/

	/**********************************************************************************************//**
	 * @fn	static int** Zeros(int height, int width)
	 *
	 * @brief	Zeros.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	height	The height.
	 * @param	width 	The width.
	 *
	 * @return	Null if it fails, else a handle to an int.
	 **************************************************************************************************/

	/*static int** Zeros(int height, int width) {
		int** result = AlllocMatrixOfInt(height,width);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				result[x][y] = 0;
			}
		}
		return result;
	}*/

	/**********************************************************************************************//**
	 * @fn	static VEC3DINT8 Zeros(int height, int width, int depth)
	 *
	 * @brief	Zeros.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	height	The height.
	 * @param	width 	The width.
	 * @param	depth 	The depth.
	 *
	 * @return	A VEC3DINT8.
	 **************************************************************************************************/

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

	/**********************************************************************************************//**
	 * @fn	static void Print2dArrayOfInt(int **mat, int width, int height)
	 *
	 * @brief	Print 2D array of int.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param [in,out]	mat   	If non-null, the matrix.
	 * @param 		  	width 	The width.
	 * @param 		  	height	The height.
	 **************************************************************************************************/

	static void Print2dArrayOfInt(int **mat, int width, int height) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width ; x++) {
				cout << mat[x][y] << ",";
			}
			cout << ";" << endl;
		}
	}

	/**********************************************************************************************//**
	 * @fn	static int BitXor(int x, int y)
	 *
	 * @brief	Bit exclusive-or.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	x	The x coordinate.
	 * @param	y	The y coordinate.
	 *
	 * @return	An int.
	 **************************************************************************************************/
	template <typename T>
	static T BitXor(T x, T y)
	{
		return x ^ y;
	}

	/**********************************************************************************************//**
	 * @fn	static uint8_t BitXor(uint8_t x, uint8_t y)
	 *
	 * @brief	Bit exclusive-or.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	x	The uint8_t to process.
	 * @param	y	The uint8_t to process.
	 *
	 * @return	An uint8_t.
	 **************************************************************************************************/

	/*static uint8_t BitXor(uint8_t x, uint8_t y)
	{
		return x ^ y;
	}*/

	/**********************************************************************************************//**
	 * @fn	static uint32_t BitXor(uint32_t x, uint32_t y)
	 *
	 * @brief	Bit exclusive-or.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	x	The uint32_t to process.
	 * @param	y	The uint32_t to process.
	 *
	 * @return	An uint32_t.
	 **************************************************************************************************/

	/*static uint32_t BitXor(uint32_t x, uint32_t y)
	{
		return x ^ y;
	}*/

	/**********************************************************************************************//**
	 * @fn	static int StringBinToDec(string bin)
	 *
	 * @brief	String bin to dec.
	 *
	 * @author	Vasa 0
	 * @date	06.01.2017
	 *
	 * @param	bin	The bin.
	 *
	 * @return	An int.
	 **************************************************************************************************/

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

	template<typename T>static void PopFront(std::vector<T>& vec){
		assert(!vec.empty());
		vec.erase(vec.begin());
	}

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

//	static string GetCompressDataString(string input) {
//		std::ofstream FILE1("cache_data.dat", std::ios::out | std::ofstream::binary);
//		FILE1 << input;//std::copy(input.begin(), input.end(), std::ostreambuf_iterator<char>(FILE1));
//		FILE1.close();
//
//		Balz *balz = new Balz("cache_data.dat", "cache_data_compress.dat");
//		balz->compress();
//		delete balz;
//
//		remove("cache_data.dat");
//
//		std::ifstream FILE2("cache_data_compress.dat", std::ios::out | std::ofstream::binary);
//		string str((std::istreambuf_iterator<char>(FILE2)), std::istreambuf_iterator<char>());
//		FILE2.close();
//		remove("cache_data_compress.dat");
//		return str;
//
//	}
//
//	static string GetCompressDataBool(vector<bool> input) {
//		FileOutputStream* fout = new FileOutputStream("cache_data.dat");
//		BitOutputStream* bout = new BitOutputStream(fout);
//		
//		for(int i = 0; i < input.size(); i++) {
//			bout->put_bit((int)input[i]);
//		}
//
//		bout->flush();
//		/*std::ofstream FILE1("cache_data.dat", std::ios::out | std::ofstream::binary);
//		std::copy(input.begin(), input.end(), std::ostreambuf_iterator<char>(FILE1));
//		FILE1.close();*/
//
//		Balz *balz = new Balz("cache_data.dat", "cache_data_compress.dat");
//		balz->compress();
//		delete balz;
//
//		remove("cache_data.dat");
//
//		std::ifstream FILE2("cache_data_compress.dat", std::ios::out | std::ofstream::binary);
//		string str((std::istreambuf_iterator<char>(FILE2)), std::istreambuf_iterator<char>());
//		FILE2.close();
//		remove("cache_data_compress.dat");
//		return str;
//
//	}
//
//	static vector<bool> GetDecompressDataBool(string input) {
//		Balz *balz = new Balz(input, "cache_decomp.dat");
//		balz->decompress();
//		delete balz;
//
//		remove(input.c_str());
//
//		vector<bool> newVec;
//		std::ifstream FILE6("cache_decomp.dat", std::ios::in | std::ifstream::binary);
//		std::istreambuf_iterator<char> iter(FILE6);
//		std::copy(iter, std::istreambuf_iterator<char>{}, std::back_inserter(newVec));
//		FILE6.close();
//		remove("cache_decomp.dat");
//
//		return newVec;
//	}
//
//	static vector<int> ConvertDequeToVector(deque<uint8_t> input){
//		vector<int> output;
//
//		for(int i = 0; i < input.size(); i++) {
//			output.push_back(input[i]);
//		}
//		return output;
//	}
//
//	static string GetCompressData(vector<int> input) {
//		std::ofstream FILE1("cache_data.dat", std::ios::out | std::ofstream::binary);
//		std::copy(input.begin(), input.end(), std::ostreambuf_iterator<char>(FILE1));
//		FILE1.close();
//
//		Balz *balz = new Balz("cache_data.dat", "cache_data_compress.dat");
//		balz->compress();
//		delete balz;
//
//		remove("cache_data.dat");
//
//		std::ifstream FILE2("cache_data_compress.dat", std::ios::out | std::ofstream::binary);
//		string str((std::istreambuf_iterator<char>(FILE2)), std::istreambuf_iterator<char>());
//		FILE2.close();
//		remove("cache_data_compress.dat");
//		return str;
//
//	}
//	static string GetDecompressDataString(string input) {
//		std::ofstream FILE8("cache_input.dat", ios::binary | ios::out);
//		FILE8 << input;
//		FILE8.close();
//
//		Balz *balz = new Balz("cache_input.dat", "cache_decomp.dat");
//		balz->decompress();
//		delete balz;
//
//		remove("cache_input.dat");
//
//		//string newVec;
//		std::ifstream FILE6("cache_decomp.dat", std::ios::in | std::ifstream::binary);
//		string newVec((std::istreambuf_iterator<char>(FILE6)), std::istreambuf_iterator<char>());
//		FILE6.close();
//		remove("cache_decomp.dat");
//
//		return newVec;
//	
//}
//	static vector<int> GetDecompressDataFromstring(string input) {
//		std::ofstream FILE8("cache_input.dat", ios::binary | ios::out);
//		FILE8 << input;
//		FILE8.close();
//
//		Balz *balz = new Balz("cache_input.dat", "cache_decomp.dat");
//		balz->decompress();
//		delete balz;
//
//		remove("cache_input.dat");
//
//		vector<int> newVec;
//		std::ifstream FILE6("cache_decomp.dat", std::ios::in | std::ifstream::binary);
//		std::istreambuf_iterator<char> iter(FILE6);
//		std::copy(iter, std::istreambuf_iterator<char>{}, std::back_inserter(newVec));
//		FILE6.close();
//		remove("cache_decomp.dat");
//
//		return newVec;
//	}
//
//
//	static vector<int> GetDecompressData(string input) {
//		Balz *balz = new Balz(input, "cache_decomp.dat");
//		balz->decompress();
//		delete balz;
//
//		remove(input.c_str());
//
//		vector<int> newVec;
//		std::ifstream FILE6("cache_decomp.dat", std::ios::in | std::ifstream::binary);
//		std::istreambuf_iterator<char> iter(FILE6);
//		std::copy(iter, std::istreambuf_iterator<char>{}, std::back_inserter(newVec));
//		FILE6.close();
//		remove("cache_decomp.dat");
//
//		return newVec;
//	}

	static vector<vector<int>> JoinsVectors(vector<ClassCodingVectors*> storage) {
		vector<vector<int>> result;
		vector<int> fullCX;
		vector<int> fullD;
		vector<int> fullP;
		vector<int> _maxp;
		int maxP = 0;
		for (int i = 0; i < storage.size(); i++) {
			vector<int> templP = storage[i]->getP();
			for (int t = 0; t < templP.size(); t++) {
				if (maxP < templP[t]) {
					maxP = templP[t];
				}
			}
		}
		_maxp.push_back(maxP);
		for (int i = 0; i < storage.size(); i++) {
			CodingVectors templ = storage[i]->getJoin();
			for (int t = 0; t < templ.CX.size(); t++) {
				fullCX.push_back(templ.CX[t]);
				fullD.push_back(templ.D[t]);
			}
			fullCX.push_back(21);
			fullD.push_back(3);

			//string str = GetCompressData(templ.D);

			vector<int> templP = storage[i]->getP();
			for (int t = 0; t < templP.size(); t++) {
				fullP.push_back(templP[t]);
			}
			fullP.push_back(maxP + 1);
		}
		result.push_back(fullCX);
		result.push_back(fullD);
		result.push_back(fullP);
		result.push_back(_maxp);

		return result;
	}

	static vector<ClassCodingVectors> SplitJoinVecotrs(vector<int> CX, vector<int> D, vector<int> P, int maxP) {
		cout << "cx: size " << CX.size() << " d: size " << D.size() << endl;
		vector<ClassCodingVectors> result;
		vector<int> _cx, _d,_p;
		for (int i = 0; i < CX.size(); i++) {
			if (CX[i] == 21 && D[i] == 3) {
				result.push_back(ClassCodingVectors(CodingVectors(_cx, _d)));
				_cx.clear();
				_d.clear();
			}
			else {
				_cx.push_back(CX[i]);
				_d.push_back(D[i]);
			}
		}
		result.push_back(ClassCodingVectors(CodingVectors(_cx, _d)));
		int counter = 0;
		for (int i = 0; i < P.size(); i++) {
			if (P[i] == maxP - 1) {
				result[counter].setP(_p);
				counter++;
				_p.clear();
			}
			else {
				_p.push_back(P[i]);
			}
		}
		result[counter].setP(_p);
		cout << result.size() << endl;
		cout << endl;
		return result;
		//ClassCodingVectors obj = ClassCodingVectors(CodingVectors(CX, D));
	}

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


	static uint8_t PopCount(uint8_t b) {

		b = ((b & 0xAA) >> 1) + (b & 0x55);
		b = ((b & 0xCC) >> 2) + (b & 0x33);
		b = ((b & 0xF0) >> 4) + (b & 0x0F);

		return b;
		}

	static uint8_t PopCount(uint16_t b) {

		b = ((b & 0xAAAA) >> 1) + (b & 0x5555);
		b = ((b & 0xCCCC) >> 2) + (b & 0x3333);
		b = ((b & 0xF0F0) >> 4) + (b & 0x0F0F);
		b = ((b & 0xFF00) >> 8) + (b & 0x00FF);

		return b;
		}

	static uint8_t PopCount(uint32_t b) {

		b = ((b & 0xAAAAAAAA) >> 1) + (b & 0x55555555);
		b = ((b & 0xCCCCCCCC) >> 2) + (b & 0x33333333);
		b = ((b & 0xF0F0F0F0) >> 4) + (b & 0x0F0F0F0F);
		b = ((b & 0xFF00FF00) >> 8) + (b & 0x00FF00FF);
		b = ((b & 0xFFFF0000) >> 16) + (b & 0x0000FFFF);

		return b;
		}



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
};
#endif 


