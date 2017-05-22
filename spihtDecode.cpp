
#include "dwtImage.hpp"
#include "spihtDecode.hpp"

void SPIHT_Decoder::initialize(int component, BitInputStream* bin) {
	lip.clear();
	lsp.clear();
	lis.clear();
	step = bin->get_bits(8);
	for (int x = 0; x < spiht.LL.w(); x++) {
		for (int y = 0; y < spiht.LL.h(); y++) {
			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::LL));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::HL));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::HL));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::HH));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::HH));

			lip.push_back(PixItem2(x, y, 0, SPIHT_Type::LH));
			lis.push_back(SetItem2(x, y, LIS_A, 0, SPIHT_Type::LH));
		}
	}
	//cout << "LIP size" << lip.size() << endl;
	//cout << "LIS size" << lis.size() << endl;
}

void SPIHT_Decoder::get_successor(int x, int y, int* sx, int* sy, int childNumber, SPIHT_Type s_type) {
	//cout << "get_successor" << endl;
	*sx = 2 * x;
	*sy = 2 * y;
	if (childNumber >= spiht.max_child) {
		*sx = -1;
		*sy = -1;
		return;
	}
	/*int lx = (im->w()) / (1 << num_stages);
	int ly = (im->h()) / (1 << num_stages);
	if(x < lx && y < ly) {
		if(x % 2 == 1)
			*sx = x + lx - 1;
		else
			*sx = x;
		if(y % 2 == 1)
			*sy = y + ly - 1;
		else
			*sy = y;
		if(*sx == x && *sy == y) {
			*sx = -1;
			*sy = -1;
			}
		}
	else {
		*sx = 2 * x;
		*sy = 2 * y;
		if(*sx >= (im->w()) || *sy >= (im->h())) {
			*sx = -1;
			*sy = -1;
			}
	}*/
}

void SPIHT_Decoder::decode(int component, int bits, BitInputStream* bin) {
	initialize(component, bin);
	int bit_cnt = 0;
	while(step >= 0) {
		cout << "==";
		/* Sorting pass */
		/* first process LIP */
		for(int i = 0; i < lip.size(); i++) {
			//cout << "zacatek1" << endl;
			bool sig = (bool)bin->get_bit();
			if(++bit_cnt > bits) return;
			if(sig) {
				lsp.push_back(PixItem2(lip[i].x, lip[i].y,lip[i].childNumber,lip[i].type_SPIHT));
				//im->set(lip[i].y, lip[i].x, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));// = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
				spiht.set(lip[i].point, lip[i].childNumber, lip[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
				if(++bit_cnt > bits) return;
				lip.erase(lip.begin() + i);
				i--;
			}
			//cout << "konec1" << endl;
		}
		/* now process LIS */
		for(int i = 0; i < lis.size(); i++) {
			//cout << "zacatek2" << endl;
			if(lis[i].type == LIS_A) {
				bool sig = (bool)bin->get_bit();
				if(++bit_cnt > bits) return;
				if(sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy,lis[i].childNumber+1,lis[i].type_SPIHT);
					/* process the four offsprings */
					sig = (bool)bin->get_bit();
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx, sy,lis[i].childNumber+1,lis[i].type_SPIHT));
						//im->set(sy, sx, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));// = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						//cout << "zacatek2 1" << endl;
						spiht.set(cv::Point(sx,sy), lis[i].childNumber+1, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						//cout << "konec2 1" << endl;
						if(++bit_cnt > bits) return;
						}
					else {
						lip.push_back(PixItem2(sx, sy,lis[i].childNumber+1,lis[i].type_SPIHT));
						}
					sig = (bool)bin->get_bit();
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx + 1, sy,lis[i].childNumber+1,lis[i].type_SPIHT));
						//im->set(sy, sx+1, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx + 1, sy) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						//cout << "zacatek2 2" << endl;
						spiht.set(cv::Point(sx+1, sy), lis[i].childNumber + 1, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						//cout << "zacatek2 2" << endl;
						if(++bit_cnt > bits) return;
						}
					else {
						lip.push_back(PixItem2(sx + 1, sy,lis[i].childNumber+1,lis[i].type_SPIHT));
						}
					sig = (bool)bin->get_bit();
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx, sy + 1,lis[i].childNumber+1,lis[i].type_SPIHT));
						//im->set(sy+1,sx, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx, sy + 1) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						spiht.set(cv::Point(sx, sy+1), lis[i].childNumber + 1, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						if(++bit_cnt > bits) return;
						}
					else {
						lip.push_back(PixItem2(sx, sy + 1,lis[i].childNumber+1,lis[i].type_SPIHT));
						}
					sig = (bool)bin->get_bit();
					if(++bit_cnt > bits) return;
					if(sig) {
						lsp.push_back(PixItem2(sx + 1, sy + 1,lis[i].childNumber+1,lis[i].type_SPIHT));
						//im->set(sy+1,sx+1, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx + 1, sy + 1) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						spiht.set(cv::Point(sx+1, sy + 1), lis[i].childNumber + 1, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						if(++bit_cnt > bits) return;
						}
					else {
						lip.push_back(PixItem2(sx + 1, sy + 1,lis[i].childNumber+1,lis[i].type_SPIHT));
						}
					/* test if L(i, j) != 0 */
					get_successor(sx, sy, &sx, &sy,lis[i].childNumber+2,lis[i].type_SPIHT);
					if(sx != -1)
						lis.push_back(SetItem2(lis[i].x, lis[i].y, LIS_B,lis[i].childNumber,lis[i].type_SPIHT));
					lis.erase(lis.begin() + i);
					i--;
					}
				}
			else {
				bool sig = (bool)bin->get_bit();
				if(++bit_cnt > bits) return;
				if(sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy,lis[i].childNumber+1,lis[i].type_SPIHT);
					lis.push_back(SetItem2(sx, sy, LIS_A,lis[i].childNumber+1,lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx + 1, sy, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.push_back(SetItem2(sx + 1, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].type_SPIHT));
					lis.erase(lis.begin() + i);
					i--;
					}
				}
			//cout << "konec2" << endl;
			}
		/* Refinement pass */
		for(int i = 0; i < lsp.size(); i++) {
			int32_t last = spiht.get(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT);
			if(std::abs((int)last) >= (1 << (step + 1))) {
				if((bool)bin->get_bit()) {
					if ((int)last >= 0) {
						//im->set(lsp[i].y, lsp[i].x,((int)last | (1 << step)) );// = (float)(((int)im->at(component, lsp[i].x, lsp[i].y)) | (1 << step));
						spiht.set(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT, ((int)last | (1 << step)));
					}
					else {
						//im->set(lsp[i].y, lsp[i].x, (-(((int)abs(im->get(lsp[i].point))) | (1 << step))));// = (float)(-(((int)std::abs((int)im->at(component, lsp[i].x, lsp[i].y))) | (1 << step)));
						spiht.set(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT, (-(((int)abs(last)) | (1 << step))));
					}
						
				}
				else {
					//im->set(lsp[i].y, lsp[i].x, ((im->get(lsp[i].point)) & (~(1 << step))));// = (float)(((int)im->at(component, lsp[i].x, lsp[i].y)) & (~(1 << step)));
					spiht.set(lsp[i].point, lsp[i].childNumber, lsp[i].type_SPIHT, (last) & (~(1 << step)));
				}
				if(++bit_cnt > bits) return;
				}
		}
		//cout << im->getCvMat() << endl;
		/* Quantization step update */
		cout << "==";
		step--;
		}
		cout << endl;
		/*cv::Mat tmp2;
		spiht.LL.getCvMat().convertTo(tmp2, CV_8UC1);
		imshow("Output", tmp2);
		cv::waitKey();*/
}

Matrix SPIHT_Decoder::getOutput(int block_size){
	Matrix out = Matrix(width, height);

	JoinSubimage join = JoinSubimage(width,height,level,1, block_size);

	join.add(DwtImage(spiht.LL.getCvMatDequant(1), level-1, 0, level, 1,Bands::LL));
	
	for (int i =0; i < level; i++) {
		//cout << "LH"<<spiht.LH[i].w() << endl;
		join.add(DwtImage(spiht.HL[level-(i+1)].getCvMatDequant(1), i, 0, level, 1,Bands::HL));
		join.add(DwtImage(spiht.LH[level- (i + 1)].getCvMatDequant(1), i, 0, level, 1, Bands::LH));
		join.add(DwtImage(spiht.HH[level- (i + 1)].getCvMatDequant(1), i, 0, level, 1, Bands::HH));
	}
	cv::Mat test = join.getJoinImage();

	/*cv::Mat tmp2;
	test.convertTo(tmp2, CV_8UC1);
	imshow("Output222", tmp2);
	cv::waitKey();*/

	return Matrix(test);
};

///
void SPIHT_Decoder_Contourlet::initialize(int component, BitInputStream* bin) {
	lip.clear();
	lsp.clear();
	lis.clear();
	step = bin->get_bits(8);

	int lowW = bin->get_bits(10);
	int lowH = bin->get_bits(10);

	matrixLow = Matrix(lowW, lowH);

	int highW = bin->get_bits(10);
	int highH = bin->get_bits(10);

	maxChild = bin->get_bits(10);
	int maxDirect = bin->get_bits(10);

	for (int i = 0; i < maxChild; i++) {
		std::vector<Matrix> temp;
		for (int k = 0; k < maxDirect; k++) {
			if (k < maxDirect / 2) {
				temp.push_back(Matrix(highW, highH));
			}
			else {
				temp.push_back(Matrix(highH, highW));
			}
			
		}
		matrixHigh.push_back(temp);
		highW *= 2;
		highH *= 2;
	}



	for (int x = 0; x < matrixLow.w(); x++) {
		for (int y = 0; y < matrixLow.h(); y++) {
			lip.push_back(PixItem3(x, y, 0, 0, SPIHT_Type::LOW));
		}
	}

	//cout << "init " << endl;
	for (int k = 0; k < matrixHigh[0].size(); k++) {
		for (int x = 0; x < matrixHigh[0][k].w(); x++) {
			for (int y = 0; y < matrixHigh[0][k].h(); y++) {

				lip.push_back(PixItem3(x, y, 0, k, SPIHT_Type::HIGH));
				lis.push_back(SetItem3(x, y, LIS_A, 0, k, SPIHT_Type::HIGH));

			}
		}
	}

	//cout << "LIP size" << lip.size() << endl;
	//cout << "LIS size" << lis.size() << endl;
}

void SPIHT_Decoder_Contourlet::get_successor(int x, int y, int* sx, int* sy, int childNumber,int direction, SPIHT_Type s_type) {
	//cout << "get_successor" << endl;
	*sx = 2 * x;
	*sy = 2 * y;
	if (childNumber >= maxChild || s_type==SPIHT_Type::LOW) {
		*sx = -1;
		*sy = -1;
		return;
	}
	if (matrixHigh[childNumber][direction].w() <= *sx || matrixHigh[childNumber][direction].h() <= *sy) {
		*sx = -1;
		*sy = -1;
		return;
	}
}

int32_t SPIHT_Decoder_Contourlet::get(cv::Point point, int childNumber, int direction, SPIHT_Type s_type) {
	if (s_type == SPIHT_Type::LOW) {
		return matrixLow.get(point);
	}
	else if (s_type == SPIHT_Type::HIGH) {
		return matrixHigh[childNumber][direction].get(point);
	}
	else {
		std::cerr << " Wrong type get" << std::endl;
		exit(-1);
	}
}

void SPIHT_Decoder_Contourlet::set(cv::Point point, int childNumber, int direction, SPIHT_Type s_type, int32_t value) {
	if (s_type == SPIHT_Type::LOW) {
		matrixLow.set(point, value);
		return;
	}
	else if (s_type == SPIHT_Type::HIGH) {
		matrixHigh[childNumber][direction].set(point,value);
		return;
	}
	else {
		std::cerr << " Wrong type set" << std::endl;
		exit(-1);
	}
}

void SPIHT_Decoder_Contourlet::decode(int component, int bits, BitInputStream* bin) {
	initialize(component, bin);
	int bit_cnt = 0;
	while (step >= 0) {
		/* Sorting pass */
		/* first process LIP */
		cout << "==";
		for (int i = 0; i < lip.size(); i++) {
			//cout << "zacatek1" << endl;
			bool sig = (bool)bin->get_bit();
			if (++bit_cnt > bits) return;
			if (sig) {
				lsp.push_back(PixItem3(lip[i].x, lip[i].y, lip[i].childNumber,lip[i].direction, lip[i].type_SPIHT));
				//im->set(lip[i].y, lip[i].x, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));// = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
				set(lip[i].point, lip[i].childNumber,lip[i].direction, lip[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
				if (++bit_cnt > bits) return;
				lip.erase(lip.begin() + i);
				i--;
			}
			//cout << "konec1" << endl;
		}
		/* now process LIS */
		for (int i = 0; i < lis.size(); i++) {
			//cout << "zacatek2" << endl;
			if (lis[i].type == LIS_A) {
				bool sig = (bool)bin->get_bit();
				if (++bit_cnt > bits) return;
				if (sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
					/* process the four offsprings */
					sig = (bool)bin->get_bit();
					if (++bit_cnt > bits) return;
					if (sig) {
						lsp.push_back(PixItem3(sx, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
						//im->set(sy, sx, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));// = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						//cout << "zacatek2 1" << endl;
						set(cv::Point(sx, sy), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						//cout << "konec2 1" << endl;
						if (++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem3(sx, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					}
					sig = (bool)bin->get_bit();
					if (++bit_cnt > bits) return;
					if (sig) {
						lsp.push_back(PixItem3(sx + 1, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
						//im->set(sy, sx+1, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx + 1, sy) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						//cout << "zacatek2 2" << endl;
						set(cv::Point(sx + 1, sy), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						//cout << "zacatek2 2" << endl;
						if (++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem3(sx + 1, sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					}
					sig = (bool)bin->get_bit();
					if (++bit_cnt > bits) return;
					if (sig) {
						lsp.push_back(PixItem3(sx, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
						//im->set(sy+1,sx, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx, sy + 1) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						set(cv::Point(sx, sy + 1), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						if (++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem3(sx, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					}
					sig = (bool)bin->get_bit();
					if (++bit_cnt > bits) return;
					if (sig) {
						lsp.push_back(PixItem3(sx + 1, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
						//im->set(sy+1,sx+1, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));//->at(component, sx + 1, sy + 1) = (float)((((bool)bin->get_bit()) ? -1 : 1) * (1 << step));
						set(cv::Point(sx + 1, sy + 1), lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT, ((((bool)bin->get_bit()) ? -1 : 1) * (1 << step)));
						if (++bit_cnt > bits) return;
					}
					else {
						lip.push_back(PixItem3(sx + 1, sy + 1, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					}
					/* test if L(i, j) != 0 */
					get_successor(sx, sy, &sx, &sy, lis[i].childNumber + 2, lis[i].direction,lis[i].type_SPIHT);
					if (sx != -1)
						lis.push_back(SetItem3(lis[i].x, lis[i].y, LIS_B, lis[i].childNumber,lis[i].direction, lis[i].type_SPIHT));
					lis.erase(lis.begin() + i);
					i--;
				}
			}
			else {
				bool sig = (bool)bin->get_bit();
				if (++bit_cnt > bits) return;
				if (sig) {
					int sx, sy;
					get_successor(lis[i].x, lis[i].y, &sx, &sy, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT);
					lis.push_back(SetItem3(sx, sy, LIS_A, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					lis.push_back(SetItem3(sx + 1, sy, LIS_A, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					lis.push_back(SetItem3(sx, sy + 1, LIS_A, lis[i].childNumber + 1, lis[i].direction,lis[i].type_SPIHT));
					lis.push_back(SetItem3(sx + 1, sy + 1, LIS_A, lis[i].childNumber + 1,lis[i].direction, lis[i].type_SPIHT));
					lis.erase(lis.begin() + i);
					i--;
				}
			}
			//cout << "konec2" << endl;
		}
		/* Refinement pass */
		for (int i = 0; i < lsp.size(); i++) {
			int32_t last = get(lsp[i].point, lsp[i].childNumber,lsp[i].direction, lsp[i].type_SPIHT);
			if (std::abs((int)last) >= (1 << (step + 1))) {
				if ((bool)bin->get_bit()) {
					if ((int)last >= 0) {
						//im->set(lsp[i].y, lsp[i].x,((int)last | (1 << step)) );// = (float)(((int)im->at(component, lsp[i].x, lsp[i].y)) | (1 << step));
						set(lsp[i].point, lsp[i].childNumber,lsp[i].direction, lsp[i].type_SPIHT, ((int)last | (1 << step)));
					}
					else {
						//im->set(lsp[i].y, lsp[i].x, (-(((int)abs(im->get(lsp[i].point))) | (1 << step))));// = (float)(-(((int)std::abs((int)im->at(component, lsp[i].x, lsp[i].y))) | (1 << step)));
						set(lsp[i].point, lsp[i].childNumber,lsp[i].direction, lsp[i].type_SPIHT, (-(((int)abs(last)) | (1 << step))));
					}

				}
				else {
					//im->set(lsp[i].y, lsp[i].x, ((im->get(lsp[i].point)) & (~(1 << step))));// = (float)(((int)im->at(component, lsp[i].x, lsp[i].y)) & (~(1 << step)));
					set(lsp[i].point, lsp[i].childNumber,lsp[i].direction, lsp[i].type_SPIHT, (last) & (~(1 << step)));
				}
				if (++bit_cnt > bits) return;
			}
		}
		//cout << im->getCvMat() << endl;
		/* Quantization step update */
		cout << "==";
		step--;
	}
	cout << endl;
	//cv::Mat tmp2;
	//matrixHigh[0][12].getCvMat().convertTo(tmp2, CV_8UC1);
	//imshow("Output", tmp2);
	//cv::waitKey();
}

MyContourlet SPIHT_Decoder_Contourlet::getOutput() {
	return MyContourlet(matrixHigh,matrixLow,maxChild);
}