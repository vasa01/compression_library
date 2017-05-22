#ifndef SPIHT_H
#define SPIHT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>



#include "matrix.hpp"
#include "tool.hpp"

#include "bitIOStream.hpp"

using namespace std;

enum SPIHT_Type { LL, HL, HH, LH, LOW, HIGH };
enum LIS_Type { LIS_A, LIS_B };

//class SPIHT_struct {
//public:
//
//private:
//	Matrix mat;
//	int childNumber;
//};

struct SPIHT {

	Matrix LL;
	vector<Matrix> HL;
	vector<Matrix> HH;
	vector<Matrix> LH;
	int max_child;
	SPIHT() {};
	SPIHT(Matrix _low, vector<Matrix> _up, vector<Matrix> _middle, vector<Matrix> _down) :LL(_low), HL(_up), LH(_down), HH(_middle) {
		max_child = HL.size();
	};
	int32_t get(cv::Point point, int childNumber, SPIHT_Type s_type ) {
		if (childNumber >= max_child) {
			cerr << "false get" <<endl;
			exit(1);
		}
		//cout << childNumber << " type:"<< (int)s_type << endl;
		if (s_type == SPIHT_Type::HL) {
			/*cout << "HL" << HL.size() << endl;
			cout << "x: " << point.x << " y: " << point.y << endl;
			int32_t test = HL[childNumber].get(point);
			cout << test << endl;
			cout << "x: " << HL[childNumber].w() << " y: " << HL[childNumber].h() << endl;
			cout << test << endl;*/
			return HL[childNumber].get(point);
		}
		else if (s_type == SPIHT_Type::LH) {
			/*cout << "LH" << LH.size() << endl;*/
			return LH[childNumber].get(point);
		}
		else if (s_type == SPIHT_Type::HH) {
			/*cout << "HH" <<HH.size()<< endl;*/
			return HH[childNumber].get(point);
		}
		else if (s_type == SPIHT_Type::LL) {
			return LL.get(point);
		}
		else {
			cerr << "type error" << endl;
			exit(1);
		}
	};

	void set(cv::Point point, int childNumber, SPIHT_Type s_type, int32_t value) {
		if (childNumber >= max_child) {
			cerr << "false set" << endl;
			exit(1);
		}
		//cout << childNumber << " type:" << (int)s_type << endl;
		if (s_type == SPIHT_Type::HL) {
			/*cout << "HL" << HL.size() << endl;
			cout << "x: " << point.x << " y: " << point.y << endl;
			int32_t test = HL[childNumber].get(point);
			cout << test << endl;
			cout << "x: " << HL[childNumber].w() << " y: " << HL[childNumber].h() << endl;
			cout << test << endl;*/
			HL[childNumber].set(point,value);
		}
		else if (s_type == SPIHT_Type::LH) {
			//cout << "LH" << LH.size() << endl;
			LH[childNumber].set(point, value);
		}
		else if (s_type == SPIHT_Type::HH) {
			//cout << "HH" << HH.size() << endl;
			HH[childNumber].set(point,value);
		}
		else if (s_type == SPIHT_Type::LL) {
			return LL.set(point, value);
		}
		else {
			cerr << "type error" << endl;
			exit(1);
		}
	};
};



struct PixItem3 {
	int x;
	int y;
	cv::Point point;
	int childNumber;
	int direction;
	SPIHT_Type type_SPIHT;
	PixItem3(int t_x, int t_y, int _childNumber, int _direction, SPIHT_Type _type_SPIHT) : x(t_x), y(t_y), point(cv::Point(t_x, t_y)), direction(_direction){
		childNumber = _childNumber;
		type_SPIHT = _type_SPIHT;
	}
};



struct SetItem3 {
	int x;
	int y;
	cv::Point point;
	int childNumber;
	int direction;
	LIS_Type type;
	SPIHT_Type type_SPIHT;
	SetItem3(cv::Point t_p, LIS_Type t_type, int _childNumber, int _direction, SPIHT_Type s_type)
		:x(t_p.x), y(t_p.y), type(t_type), point(t_p), childNumber(_childNumber), type_SPIHT(s_type), direction(_direction)
	{ }

	SetItem3(int t_x, int t_y, LIS_Type t_type, int _childNumber, int _direction, SPIHT_Type s_type)
		:x(t_x), y(t_y), type(t_type), point(cv::Point(t_x, t_y)), childNumber(_childNumber), type_SPIHT(s_type), direction(_direction)
	{ }
};

struct PixItem2{
	int x;
	int y;
	cv::Point point;
	int childNumber;
	SPIHT_Type type_SPIHT;
	PixItem2(int t_x, int t_y, int _childNumber, SPIHT_Type _type_SPIHT) : x(t_x), y(t_y), point(cv::Point(t_x, t_y)) {
		childNumber = _childNumber; 
		type_SPIHT = _type_SPIHT;
	}
};



struct SetItem2{
	int x;
	int y;
	cv::Point point;
	int childNumber;
	LIS_Type type;
	SPIHT_Type type_SPIHT;
	SetItem2(cv::Point t_p, LIS_Type t_type, int _childNumber, SPIHT_Type s_type)
		:x(t_p.x), y(t_p.y), type(t_type), point(t_p), childNumber(_childNumber), type_SPIHT(s_type)
		{ }

	SetItem2(int t_x, int t_y, LIS_Type t_type, int _childNumber, SPIHT_Type s_type)
		:x(t_x), y(t_y), type(t_type), point(cv::Point(t_x,t_y)), childNumber(_childNumber), type_SPIHT(s_type)
		{ }
};

typedef vector<PixItem2> LIP;
typedef vector<PixItem2> LSP;
typedef vector<SetItem2> LIS;

typedef vector<PixItem3> LIP3;
typedef vector<PixItem3> LSP3;
typedef vector<SetItem3> LIS3;


#endif