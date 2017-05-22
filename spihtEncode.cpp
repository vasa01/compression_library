
#include "spihtEncode.hpp"


//#include "utils.h"

void SPIHT_Encoder::initialize( BitOutputStream* bout) {
	lip.clear();
	lsp.clear();
	lis.clear();
	//cout << "init" << endl;
	Matrix absMat = Tool::Abs(*im);
	
	int32_t max = absMat.getMax();
	step = (int)floor(log((float)max-1) / log(2.0));
	bout->put_bits(step, 8);
	//cout << "init "<<spiht.HH[0].w() << endl;
	for (int x = 0; x < spiht.LL.w(); x++) {
		for (int y = 0; y < spiht.LL.h(); y++) {
			lip.push_back(PixItem2(x,y,0, SPIHT_Type::LL));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::HL));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::HL));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::HH));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::HH));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::LH));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::LH));
		}
	}
	//for(int y = 0; y < subIm[0].h(); y++)
	//	for(int x = 0; x < subIm[0].w(); x++) {
	//		//cout << "x=" << x << " y=" << y << endl;
	//		//množina indexù(souøadnic) koøenù.Spadají sem koeficienty z nejvyšší úrovnì rozkladu, pøípadnì též DC koeficient
	//		lip.push_back(PixItem(x, y));
	//		if(x >= (subIm[0].w() / (2)) || y >= (subIm[0].h() / (2))){//if ((x % 2 != 0) || (y % 2 != 0)) {
	//			//cout << "push" << endl;
	//			//pouze ty prvky z lip, které mají pøímé potomky, jako prvky typu A
	//			lis.push_back(SetItem(x, y, LIS_A));
	//		}
	//}
	//cout << "LIP size" << lip.size() << endl;
	//cout << "LIS size" << lis.size() << endl;
	/*for (int i = 0; i < lip.size(); i++) {
		cout << "(" << lip[i].x << "," << lip[i].y << ");";
	}
	cout << endl;*/
}

void SPIHT_Encoder::get_successor(int x, int y, int* sx, int* sy, int childNumber, SPIHT_Type s_type) {
	//cout << "get_successor" << endl;
	*sx = 2 * x;
	*sy = 2 * y;
	if (childNumber >= spiht.max_child) {
		*sx = -1;
		*sy = -1;
		return;
	}

}


bool SPIHT_Encoder::is_significant_pixel( int x, int y,int childNumber, SPIHT_Type s_type) {
	//cout << "signi" << endl;
	bool result =  std::abs(spiht.get(cv::Point(x, y), childNumber, s_type)) >= (1 << step);
	//cout << "signi2" << endl;
	return result;
	/*if (s_type == SPIHT_Type::HL) {
		return std::abs(spiht.HL[childNumber].get(y,x)) >= (1 << step);
	}else if (s_type == SPIHT_Type::LH) {
		return std::abs(spiht.LH[childNumber].get(y, x)) >= (1 << step);
	}else if (s_type == SPIHT_Type::HH) {
		return std::abs(spiht.HH[childNumber].get(y, x)) >= (1 << step);
	}
	else if (s_type == SPIHT_Type::LL) {
		return std::abs(spiht.LL.get(y, x)) >= (1 << step);
	}
	else {
		cerr << "type error" << endl;
		exit(1);
	}*/
	//return ;// (std::abs(im->get(y, x)) >= (1 << step));
}

bool SPIHT_Encoder::is_significant_set_A( int x, int y, int childrenNumber, SPIHT_Type s_type, int count) {
	//cout << "is_significant_set_A" << endl;
	if(count > 1 && is_significant_pixel( x, y, childrenNumber,s_type))
		return true;
	int sx, sy;
	get_successor(x, y, &sx, &sy, childrenNumber+1, s_type);
	if(sx == -1 || sy == -1)
		return false;
	//cout << "is_significant_set_A 2" << endl;
	if(is_significant_set_A( sx, sy, childrenNumber+1, s_type, count + 1))
		return true;
	else if(is_significant_set_A( sx + 1, sy, childrenNumber + 1, s_type, count + 1))
		return true;
	else if(is_significant_set_A( sx, sy + 1, childrenNumber + 1, s_type, count + 1))
		return true;
	else if(is_significant_set_A( sx + 1, sy + 1, childrenNumber + 1, s_type, count + 1))
		return true;
	return false;
}

bool SPIHT_Encoder::is_significant_set_B( int x, int y, int childrenNumber, SPIHT_Type s_type,int count) {
	if(count > 2 && is_significant_pixel(x, y, childrenNumber, s_type))
		return true;
	int sx, sy;
	get_successor(x, y, &sx, &sy, childrenNumber+1,s_type);
	if(sx == -1 || sy == -1)
		return false;
	if(is_significant_set_B( sx, sy, childrenNumber + 1, s_type,count + 1))
		return true;
	else if(is_significant_set_B( sx + 1, sy, childrenNumber + 1, s_type, count + 1))
		return true;
	else if(is_significant_set_B( sx, sy + 1, childrenNumber + 1, s_type,count + 1))
		return true;
	else if(is_significant_set_B( sx + 1, sy + 1, childrenNumber + 1, s_type,count + 1))
		return true;
	return false;
}

void SPIHT_Encoder::encode( int bits,  BitOutputStream* bout) {
	initialize( bout);
	//cout << "po innit" << endl;
	int bit_cnt = 0;
	//int tik = step/20;
	//string text = ".";
	while(step >= 0) {
		/* Sorting pass */
		/* first process LIP */
		//if (step % tik == 0) {
		cout << "==";
		cout <<step;
		//	//text += ".";
		//}
		for(int i = 0; i < lip.size(); i++) {
			//lip[i].x;
			//cout << "zacatek1" << endl;
			bool sig = is_significant_pixel( lip[i].x, lip[i].y, lip[i].childNumber,lip[i].type_SPIHT);
			//cout << "zacatek1" << endl;
			bout->put_bit((int)sig);
			if(++bit_cnt > bits) return;
			if(sig) {
				lsp.push_back(PixItem2(lip[i].x, lip[i].y,lip[i].childNumber,lip[i].type_SPIHT));
				//cout << "LIP: sig == 1 lsp push back bit value: "<<(((int)im->get(lip[i].y, lip[i].x)) > 0 ? 0 : 1) << endl;
				bout->put_bit((int) (spiht.get(lip[i].point,lip[i].childNumber,lip[i].type_SPIHT) > 0 ? 0 : 1));
				if(++bit_cnt > bits) return;
				lip.erase(lip.begin() + i);
				i--;
			}
			//cout << "konec1"  << endl;
		}
		/*cout << " LIP:";
		for(int i = 0; i < lip.size(); i++) {
			cout << im->get(lip[i].point) << "|";
			}
		cout << endl;*/
		/* now process LIS */
		

		for(int i = 0; i < lis.size(); i++) {
			//cout << "zacatek2" << endl;
			if(lis[i].type == LIS_A) {
				//cout << "LIS_A:!!!" << endl;
				bool sig = is_significant_set_A( lis[i].x, lis[i].y, lis[i].childNumber, lis[i].type_SPIHT);
				//cout << "LIS_A:  bit value: " << (int)sig << endl;
				bout->put_bit((int)sig);
				if(++bit_cnt > bits) return;
				if(sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy, lis[i].childNumber+1,lis[i].type_SPIHT);
					/* process the four offsprings */
					sig = is_significant_pixel( sx, sy,lis[i].childNumber+1,lis[i].type_SPIHT);
					//cout << "LIS_A>: sig == 1 value(sx,sy)"<< im->get(sy,sx)<< " bit value:" << (int)sig << endl;
					bout->put_bit((int)sig);
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx, sy, lis[i].childNumber + 1, lis[i].type_SPIHT));
						bout->put_bit((int)(spiht.get(cv::Point(sx,sy), lis[i].childNumber+1, lis[i].type_SPIHT) > 0 ? 0 : 1));
						//cout << "LIS_A>>: sig == 1 lsp push back bit value: " << ((im->get(sy, sx)) > 0 ? 0 : 1) << endl;
						if(++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem2(sx, sy, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 0 lip push back value: "<< im->get(sy, sx) << endl;
					}
					//sx+1
					sig = is_significant_pixel( sx + 1, sy, lis[i].childNumber + 1, lis[i].type_SPIHT);
					//cout << "LIS_A>: sig == 1 value(sx+1,sy) " << im->get(sy, sx+1) << " bit value: " << (int)sig << endl;
					bout->put_bit((int)sig);
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx + 1, sy, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 1 lsp push back value(sx+1,sy) " << im->get(sy, sx+1) << " bit value: " << (int)sig << endl;
						bout->put_bit((int)(spiht.get(cv::Point(sx+1, sy), lis[i].childNumber + 1, lis[i].type_SPIHT) > 0 ? 0 : 1));
						if(++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem2(sx + 1, sy, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 0 value(sx+1,sy) " << im->get(sy, sx+1) << "LIP push back" << endl;
					}
					//sy+1
					sig = is_significant_pixel( sx, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT);
					//cout << "LIS_A>: sig == 1 value(sx,sy+1) " << im->get(sy+1, sx) << " bit value: " << (int)sig << endl;
					bout->put_bit((int)sig);
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 1 lsp push back value(sx,sy+1) " << im->get(sy+1, sx) << " bit value:" << (int)sig << endl;
						bout->put_bit((int)(spiht.get(cv::Point(sx, sy+1), lis[i].childNumber + 1, lis[i].type_SPIHT) > 0 ? 0 : 1));
						if(++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem2(sx, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 0 value(sx,sy+1) " << im->get(sy+1, sx) << "LIP push back" << endl;
					}
					//sx+1 sy+1
					sig = is_significant_pixel( sx + 1, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT);
					//cout << "LIS_A>: sig == 1 value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << " bit value: " << (int)sig << endl;
					bout->put_bit((int)sig);
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx + 1, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 1 lsp push back value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << " bit value: " << (int)sig << endl;
						bout->put_bit((int)(spiht.get(cv::Point(sx+1, sy+1), lis[i].childNumber + 1, lis[i].type_SPIHT) > 0 ? 0 : 1));
						if(++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem2(sx + 1, sy + 1, lis[i].childNumber + 1, lis[i].type_SPIHT));
						//cout << "LIS_A>>: sig == 0 value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << "LIP push back" << endl;
					}
					/* test if L(i, j) != 0 */
					get_successor(sx, sy, &sx, &sy, lis[i].childNumber+2,lis[i].type_SPIHT);
					if(sx != -1)
						lis.push_back(SetItem2(lis[i].x, lis[i].y, LIS_B, lis[i].childNumber , lis[i].type_SPIHT));//????
					lis.erase(lis.begin() + i);
					i--;
				}
			}
			else {
				//cout << "LIS_B:" << endl;
				bool sig = is_significant_set_B( lis[i].x, lis[i].y, lis[i].childNumber, lis[i].type_SPIHT);
				//cout << "LIS_B:  bit value: " << (int)sig << endl;
				bout->put_bit((int)sig);
				if(++bit_cnt > bits) return;
				if(sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy,lis[i].childNumber+1,lis[i].type_SPIHT);
					lis.push_back(SetItem2(sx, sy, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx + 1, sy, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx + 1, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.erase(lis.begin() + i);
					i--;
				}
			}
			//cout << "konec2" << endl;
		}
		
		
		for(int i = 0; i < lsp.size(); i++) {
			if(abs(spiht.get(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT)) >= (1 << (step + 1))) {
				bout->put_bit((((abs(spiht.get(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT))) >> step)) & 1);
				if(++bit_cnt > bits) return;
			}
		}
		cout << "==";
		step--;
	}
	cout << endl;
}

	void SPIHT_Encoder_Contourlet::initialize(BitOutputStream* bout) {
		lip.clear();
		lsp.clear();
		lis.clear();

		int32_t maxCon = 0;
		double min, max;
		std::vector<std::vector<cv::Mat>> high = contourlet.getHigh();
		cv::Mat low = contourlet.getLow();
		matrixLow = Matrix(low);
		cv::minMaxLoc(cv::abs(low), &min, &max);
		maxCon = static_cast<int32_t>(max);
		maxChild = high.size();
		int index = 0;
		for (int i = high.size()-1; i > -1; i--) {
			std::vector<Matrix> temp;
			for (int k = 0; k < high[i].size(); k++) {
				temp.push_back(Matrix(high[i][k]));
				//cout << "width: " << high[i][k].cols << " height: " << high[i][k].rows << endl;
				cv::minMaxLoc(cv::abs(high[i][k]), &min, &max);
				if (maxCon < max) {
					maxCon = static_cast<int32_t>(max);
				}
			}
			matrixHigh.push_back(temp);
		}
		step = (int)floor(log((float)maxCon-1) / log(2.0));
		bout->put_bits(step, 8);

		bout->put_bits(matrixLow.w(), 10);
		bout->put_bits(matrixLow.h(), 10);

		bout->put_bits(matrixHigh[0][0].w(), 10);
		bout->put_bits(matrixHigh[0][0].h(), 10);

		bout->put_bits(matrixHigh.size(),10);
		bout->put_bits(matrixHigh[0].size(), 10);

		for (int x = 0; x < matrixLow.w(); x++) {
			for (int y = 0; y < matrixLow.h(); y++) {
				lip.push_back(PixItem3(x, y, 0, 0,SPIHT_Type::LOW));
			}
		}

		//cout << "init " << endl;
		for (int k = 0; k < matrixHigh[0].size(); k++) {
			//cout << "width: " << matrixHigh[0][k].w() << " height: " << matrixHigh[0][k].h() << endl;
			for (int x = 0; x < matrixHigh[0][k].w(); x++) {
				for (int y = 0; y < matrixHigh[0][k].h(); y++) {

					lip.push_back(PixItem3(x, y, 0,k, SPIHT_Type::HIGH));
					lis.push_back(SetItem3(x, y, LIS_A, 0,k, SPIHT_Type::HIGH));

				}
			}
		}

		//cout << "LIP size" << lip.size() << endl;
		//cout << "LIS size" << lis.size() << endl;

	}

	void SPIHT_Encoder_Contourlet::get_successor(int x, int y, int* sx, int* sy, int childNumber, int direction, SPIHT_Type s_type) {
		//cout << "get_successor" << endl;
		*sx = 2 * x;
		*sy = 2 * y;
		if (childNumber >= maxChild || s_type == SPIHT_Type::LOW) {
			*sx = -1;
			*sy = -1;
			return;
		}
		if (matrixHigh[childNumber][direction].w() <= *sx || matrixHigh[childNumber][direction].h()<=*sy) {
			*sx = -1;
			*sy = -1;
			return;
		}
		

	}

	bool SPIHT_Encoder_Contourlet::is_significant_pixel(int x, int y, int childNumber,int direction, SPIHT_Type s_type) {
		//cout << "child: "<< childNumber << endl;
		bool result;
		if (s_type == SPIHT_Type::LOW) {		
			result = std::abs(matrixLow.get(cv::Point(x, y))) >= (1 << step);
		}
		else if(s_type == SPIHT_Type::HIGH) {
			//cout << "point: " << x << " : " << y<<" w: "<< matrixHigh[childNumber][direction].w()<< " h: "<< matrixHigh[childNumber][direction].h() << endl;
			result = std::abs(matrixHigh[childNumber][direction].get(cv::Point(x, y))) >= (1 << step);
		}
		else {
			std::cerr << " Wrong type significant" << std::endl;
			exit(-1);
		}
		 
		//cout << "signi2" << endl;
		return result;
	}

	bool SPIHT_Encoder_Contourlet::is_significant_set_A(int x, int y, int childrenNumber,int direction, SPIHT_Type s_type, int count) {
		//cout << "is_significant_set_A" << endl;
		if (count > 1 && is_significant_pixel(x, y, childrenNumber, direction, s_type))
			return true;
		int sx, sy;
		get_successor(x, y, &sx, &sy, childrenNumber + 1, direction, s_type);
		//cout << "s_point: " << sx << " : " << sy << endl;
		if (sx == -1 || sy == -1)
			return false;
		//cout << "is_significant_set_A 2" << endl;
		if (is_significant_set_A(sx, sy, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		else if (is_significant_set_A(sx + 1, sy, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		else if (is_significant_set_A(sx, sy + 1, childrenNumber + 1, direction,s_type, count + 1))
			return true;
		else if (is_significant_set_A(sx + 1, sy + 1, childrenNumber + 1, direction,s_type, count + 1))
			return true;
		return false;
	}

	bool SPIHT_Encoder_Contourlet::is_significant_set_B(int x, int y, int childrenNumber,  int direction, SPIHT_Type s_type, int count) {
		if (count > 2 && is_significant_pixel(x, y, childrenNumber, direction, s_type))
			return true;
		int sx, sy;
		get_successor(x, y, &sx, &sy, childrenNumber + 1, direction, s_type);
		if (sx == -1 || sy == -1)
			return false;
		if (is_significant_set_B(sx, sy, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		else if (is_significant_set_B(sx + 1, sy, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		else if (is_significant_set_B(sx, sy + 1, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		else if (is_significant_set_B(sx + 1, sy + 1, childrenNumber + 1, direction, s_type, count + 1))
			return true;
		return false;
	}

	int32_t SPIHT_Encoder_Contourlet::get(cv::Point point, int childNumber, int direction, SPIHT_Type s_type) {
		//cout << "get" << endl;
		if (s_type == SPIHT_Type::LOW) {
			//cout << "point: " << point.x << " : " << point.y << endl;
			return matrixLow.get(point);
		}
		else if (s_type == SPIHT_Type::HIGH) {
			//cout << "point: " << point.x << " : " << point.y << endl;
			return matrixHigh[childNumber][direction].get(point);
		}
		else {
			std::cerr << " Wrong type get" << std::endl;
			exit(-1);
		}
	}

	void SPIHT_Encoder_Contourlet::encode(int bits, BitOutputStream* bout) {
		initialize(bout);
		
		int bit_cnt = 0;
		//int tik = step / 10;
		//string text = ".";
		while (step >= 0) {
			//cout << "while" << endl;
			/* Sorting pass */
			/* first process LIP */
			/*if (step % tik == 0) {
				cout << text << endl;
				text += ".";
			}*/
			//cout << "while" << endl;
			cout << "==";
			for (int i = 0; i < lip.size(); i++) {
				//lip[i].x;
				//cout << "zacatek1" << endl;
				bool sig = is_significant_pixel(lip[i].x, lip[i].y, lip[i].childNumber,lip[i].direction, lip[i].type_SPIHT);
				//cout << "zacatek1" << endl;
				bout->put_bit((int)sig);
				if (++bit_cnt > bits) return;
				if (sig) {
					lsp.push_back(PixItem3(lip[i].x, lip[i].y, lip[i].childNumber,lip[i].direction, lip[i].type_SPIHT));
					//cout << "LIP: sig == 1 lsp push back bit value: "<<(((int)im->get(lip[i].y, lip[i].x)) > 0 ? 0 : 1) << endl;
					bout->put_bit((int)(get(lip[i].point, lip[i].childNumber,lip[i].direction, lip[i].type_SPIHT) > 0 ? 0 : 1));
					if (++bit_cnt > bits) return;
					lip.erase(lip.begin() + i);
					i--;
				}
				//cout << "konec1"  << endl;
			}
			/*cout << " LIP:";
			for(int i = 0; i < lip.size(); i++) {
			cout << im->get(lip[i].point) << "|";
			}
			cout << endl;*/
			/* now process LIS */


			for (int i = 0; i < lis.size(); i++) {
				//cout << "zacatek2" << endl;
				if (lis[i].type == LIS_A) {
					//cout << "LIS_A:!!!" << endl;
					bool sig = is_significant_set_A(lis[i].x, lis[i].y, lis[i].childNumber,lis[i].direction, lis[i].type_SPIHT);
					//cout << "LIS_A:  bit value: " << (int)sig << endl;
					bout->put_bit((int)sig);
					if (++bit_cnt > bits) return;
					if (sig) {
						int sx, sy;
						get_successor(lis[i].x, lis[i].y, &sx, &sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						/* process the four offsprings */
						sig = is_significant_pixel(sx, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						//cout << "LIS_A>: sig == 1 value(sx,sy)"<< im->get(sy,sx)<< " bit value:" << (int)sig << endl;
						bout->put_bit((int)sig);
						if (++bit_cnt > bits) return;
						if (sig) {
							lsp.push_back(PixItem3(sx, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							bout->put_bit((int)(get(cv::Point(sx, sy), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT) > 0 ? 0 : 1));
							//cout << "LIS_A>>: sig == 1 lsp push back bit value: " << ((im->get(sy, sx)) > 0 ? 0 : 1) << endl;
							if (++bit_cnt > bits) return;
						}
						else {
							lip.push_back(PixItem3(sx, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 0 lip push back value: "<< im->get(sy, sx) << endl;
						}
						//sx+1
						sig = is_significant_pixel(sx + 1, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						//cout << "LIS_A>: sig == 1 value(sx+1,sy) " << im->get(sy, sx+1) << " bit value: " << (int)sig << endl;
						bout->put_bit((int)sig);
						if (++bit_cnt > bits) return;
						if (sig) {
							lsp.push_back(PixItem3(sx + 1, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 1 lsp push back value(sx+1,sy) " << im->get(sy, sx+1) << " bit value: " << (int)sig << endl;
							bout->put_bit((int)(get(cv::Point(sx + 1, sy), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT) > 0 ? 0 : 1));
							if (++bit_cnt > bits) return;
						}
						else {
							lip.push_back(PixItem3(sx + 1, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 0 value(sx+1,sy) " << im->get(sy, sx+1) << "LIP push back" << endl;
						}
						//sy+1
						sig = is_significant_pixel(sx, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						//cout << "LIS_A>: sig == 1 value(sx,sy+1) " << im->get(sy+1, sx) << " bit value: " << (int)sig << endl;
						bout->put_bit((int)sig);
						if (++bit_cnt > bits) return;
						if (sig) {
							lsp.push_back(PixItem3(sx, sy + 1, lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 1 lsp push back value(sx,sy+1) " << im->get(sy+1, sx) << " bit value:" << (int)sig << endl;
							bout->put_bit((int)(get(cv::Point(sx, sy + 1), lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT) > 0 ? 0 : 1));
							if (++bit_cnt > bits) return;
						}
						else {
							lip.push_back(PixItem3(sx, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 0 value(sx,sy+1) " << im->get(sy+1, sx) << "LIP push back" << endl;
						}
						//sx+1 sy+1
						sig = is_significant_pixel(sx + 1, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						//cout << "LIS_A>: sig == 1 value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << " bit value: " << (int)sig << endl;
						bout->put_bit((int)sig);
						if (++bit_cnt > bits) return;
						if (sig) {
							lsp.push_back(PixItem3(sx + 1, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 1 lsp push back value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << " bit value: " << (int)sig << endl;
							bout->put_bit((int)(get(cv::Point(sx + 1, sy + 1), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT) > 0 ? 0 : 1));
							if (++bit_cnt > bits) return;
						}
						else {
							lip.push_back(PixItem3(sx + 1, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
							//cout << "LIS_A>>: sig == 0 value(sx+1,sy+1) " << im->get(sy + 1, sx+1) << "LIP push back" << endl;
						}
						/* test if L(i, j) != 0 */
						get_successor(sx, sy, &sx, &sy, lis[i].childNumber + 2, lis[i].direction,lis[i].type_SPIHT);
						if (sx != -1)
							lis.push_back(SetItem3(lis[i].x, lis[i].y, LIS_B, lis[i].childNumber,lis[i].direction, lis[i].type_SPIHT));//????
						lis.erase(lis.begin() + i);
						i--;
					}
				}
				else {
					//cout << "LIS_B:" << endl;
					bool sig = is_significant_set_B(lis[i].x, lis[i].y, lis[i].childNumber, lis[i].direction, lis[i].type_SPIHT);
					//cout << "LIS_B:  bit value: " << (int)sig << endl;
					bout->put_bit((int)sig);
					if (++bit_cnt > bits) return;
					if (sig) {
						int sx, sy;
						get_successor(lis[i].x, lis[i].y, &sx, &sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
						lis.push_back(SetItem3(sx, sy, LIS_A, lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT));
						lis.push_back(SetItem3(sx + 1, sy, LIS_A, lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT));
						lis.push_back(SetItem3(sx, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT));
						lis.push_back(SetItem3(sx + 1, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].direction, lis[i].type_SPIHT));
						lis.erase(lis.begin() + i);
						i--;
					}
				}
				//cout << "konec2" << endl;
			}


			for (int i = 0; i < lsp.size(); i++) {
				if (abs(get(lsp[i].point, lsp[i].childNumber, lsp[i].direction, lsp[i].type_SPIHT)) >= (1 << (step + 1))) {
					bout->put_bit((((abs(get(lsp[i].point, lsp[i].childNumber, lsp[i].direction, lsp[i].type_SPIHT))) >> step)) & 1);
					if (++bit_cnt > bits) return;
				}
			}
			cout << "==";
			step--;
		}
		cout << endl;
		//cv::Mat tmp2;
		//matrixHigh[0][12].getCvMat().convertTo(tmp2, CV_8UC1);
		//imshow("Input", tmp2);

	}