#include "compressionLibrary.hpp"

using namespace COMPR_LBR;

	void compresFile(string inputSource, string outputSource, COLOR_SPACE_TYPE codeColorType, TRANS_TYPE transType, LEVEL_NUM levelNum, CODE_TYPE codeType, vector<bool> set, vector<int> setValue, int block_size) {
		//bool pokus = false;
		//cout << "size in KB: "<< kilobytes << "KB" << endl;
		//double rate = procent*0.01;
		cv::Mat image;
		int maxChannel;
		int tilesSize=0;
		
		CDF97 *cdf97 = new CDF97(levelNum, image);
		CDF53 *cdf53 = new CDF53(levelNum, image);
		
		MyContourlet mycontourlet;
		std::vector<MyContourlet> contur;

		std::vector<std::vector<DwtImage*>> ebcotTiles;

		std::cout << "Comress" << endl;

		if (codeColorType == COLOR_SPACE_TYPE::GRAY) {
			image = cv::imread(inputSource, CV_LOAD_IMAGE_GRAYSCALE);
		}
		else {
			image = cv::imread(inputSource, CV_LOAD_IMAGE_COLOR);
		}

		//assert(!image.empty());

		//std::cout << ">>> ColorSpace modul begin" << endl;
		if (image.empty()) {
			cerr << "Error: Unable to open input file. (" << inputSource << ")" << endl;
			exit(1);
		}

		if(levelNum == 0) {
			levelNum = (LEVEL_NUM)static_cast<int32_t>(floor(log10(image.cols) / log10(2.0)));
			//cout << (int)levelNum << endl;
		}
		
		if(block_size > image.cols) {
			block_size = image.cols;
		}

		bool threeChanels = image.channels() == 3 ? 1 : 0;
		cv::Mat coloredMat;
		if (threeChanels) {
			//std::cout << "image have 3 channels" << endl;
			if (codeColorType == COLOR_SPACE_TYPE::YCBCR) {
				coloredMat = ConvertBRG2YCbCr(image);
			}
			else if (codeColorType == COLOR_SPACE_TYPE::GRAY) {
				coloredMat = ConvertBRG2GRAY(image);
			}
			else {//BGR
				coloredMat = image.clone();
			}
		}
		else {
			if (codeColorType != COLOR_SPACE_TYPE::GRAY) {
				cerr << "Error: You can not convert a grayscale image to color." << endl;
				exit(1);
			}
			else {
				coloredMat = image.clone();
			}
		}
		//std::cout << "<<< ColorSpace modul done!" << endl;
		if (transType == TRANS_TYPE::DWT_CDF97) {
			//************ CDF 9/7 ***********************//
			//std::cout << ">>> Cdf 9 / 7 begin" << endl;
			maxChannel = coloredMat.channels();

			cdf97 = new CDF97(levelNum, coloredMat, block_size);
			cdf97->forward2d(coloredMat);
			cv::Mat imageAfterCDF97 = cdf97->getForward();
			imwrite("inverse_2.bmp", imageAfterCDF97);
			maxChannel = cdf97->getLevelsObject()->getMaxChanel();

			//std::cout << "<<< Cdf 9 / 7 done!" << endl;
			//******* coding dwt image ********************//
		}else if(transType == TRANS_TYPE::DWT_CDF53) {
			//************ CDF 9/7 ***********************//
			std::cout << ">>> Cdf 5/3 begin" << endl;
			maxChannel = coloredMat.channels();

			cdf53 = new CDF53(levelNum, coloredMat, block_size);
			cdf53->forward2d(coloredMat);
			cv::Mat imageAfterCDF53 = cdf53->getForward();
			imwrite("inverse_2.bmp", imageAfterCDF53);
			maxChannel = cdf53->getLevelsObject()->getMaxChanel();

			std::cout << "<<< Cdf 5/3 done!" << endl;
			//******* coding dwt image ********************//
		}
		else if (transType == TRANS_TYPE::CON) {
			maxChannel = coloredMat.channels();
			cv::Mat *channel = new cv::Mat[maxChannel];

			if (maxChannel == 3) {
				split(coloredMat, channel);
			}
			else {
				channel[0] = coloredMat;
			}
			for (int ch = 0; ch < maxChannel; ch++) {
				int h, w;
				ivec dfb_levels;
				contourlet_t *contourlet;

				std::cout << ">>> Contourlet begin" << endl;
				mat source = loadMat(channel[ch]);
				std::cout << "created" << std::endl;
				dfb_levels = ivec_new(levelNum);
				for (int i = 0; i < levelNum; i++) {
					dfb_levels[i] = 4;
				}

				contourlet = contourlet_new((int)levelNum, dfb_levels);
				std::cout << "created" << std::endl;
				contourlet_transform(contourlet, source);

				mycontourlet = MyContourlet(contourlet);
				std::cout << "created" << std::endl;
				contur.push_back(mycontourlet);

				contourlet_delete(contourlet);
				ivec_delete(dfb_levels);
				mat_delete(source);
				std::cout << ">>> Contourlet done!" << endl;
			}
		}
		FileOutputStream* fout = new FileOutputStream(outputSource.c_str());
		BitOutputStream* bout = new BitOutputStream(fout);


		int sumInformData = 0;

		if (codeType == CODE_TYPE::EZW && (transType == TRANS_TYPE::DWT_CDF97 || transType == TRANS_TYPE::DWT_CDF53)) {
			//EZW coding
			std::cout << ">>> EZW coding begin" << endl;

			int min_threshold= 1 << setValue[0];

			bout->put_bits('_', 8);
			bout->put_bits('E', 8);
			bout->put_bits('Z', 8);
			bout->put_bits('W', 8);
			bout->put_bits('_', 8);
			bout->put_bits(transType, 6);
			bout->put_bits(levelNum, 6);
			bout->put_bits(codeColorType, 6);
			bout->put_bits(maxChannel, 6);
			bout->put_bits(image.cols, 12);
			bout->put_bits(image.rows, 12);
			sumInformData += 8 * 5 + 6 * 4 + 12 * 2;

			cv::Mat *channel = new cv::Mat[maxChannel];
			cv::Mat input;
			if(transType == TRANS_TYPE::DWT_CDF97) {
				input = cdf97->getForward();
			}
			else {
				input = cdf53->getForward();
			}

			if (maxChannel == 3) {
				split(input, channel);
			}
			else {
				channel[0] = input;
				/*cv::Mat result = channel[0].clone();
				cv::Mat test;
				result.convertTo(test, CV_8UC1);
				imshow("EZW", test);
				cv::waitKey();*/
			}

			for (int ch = 0; ch < maxChannel; ch++) {//vectorImages.size()

				cout << "ezw_channel_"<< ch << endl;
				EZWEncode encode = EZWEncode();
				encode.encode(Matrix(channel[ch], 1), min_threshold);//vectorImages[i]->getImage()
				ostringstream temp;
				BitOutputStream* ezwOut = new BitOutputStream(&temp);
				ostringstream* streams = encode.getBitstreamString();
				auto encoded1 = streams->str();
				BitOutputStream* streamsCount = encode.getBitstream();
				int countEncoded1 = streamsCount->getActualLen();

				
				deque<uint8_t> symbols = encode.getSymbols();

				//string str = Tool::GetCompressData(Tool::ConvertDequeToVector(symbols));

				//cout << "symbols: " << symbols.size() << endl;
				//cout << "Threshold: " << encode.getThreshold() << endl;
				int size = symbols.size();

				unsigned numCodingSymbols = 4;
				auto ae = ARIEncode(ezwOut, numCodingSymbols);
				for (auto val : symbols) {
					ae.encode(val);
				}
				ae.close();

				auto encoded2 = temp.str();
				int countEncoded2 = ezwOut->getActualLen();

				bout->put_bits(min_threshold, 12);
				bout->put_bits(countEncoded1, 32);
				bout->put_bits(countEncoded2, 32); //len
				bout->put_bits(numCodingSymbols, 8);
				bout->put_bits(size, 32);
				bout->put_bits(channel[ch].cols, 12); //vectorImages[i]->w()
				bout->put_bits(channel[ch].rows, 12);//vectorImages[i]->h()
				bout->put_bits(encode.getThreshold(), 32);

				sumInformData += 32 * 4 + 8 + 2 * 12;

				istringstream is1(encoded1);
				BitInputStream* bis1 = new BitInputStream(&is1);
				for (int b = 0; b < countEncoded1; b++) {
					bout->put_bit(bis1->get_bit());
				}

				istringstream is2(encoded2);
				BitInputStream* bis2 = new BitInputStream(&is2);
				for (int b = 0; b < countEncoded2; b++) {
					bout->put_bit(bis2->get_bit());
				}
			}
			bout->flush();


			std::cout << "<<< EZW coding done!" << endl;
			std::cout << "BitstreamFileSize:............................" << (bout->getActualLen() - sumInformData) / 8000.0 << "KB" << endl;

		}
		else if (codeType == CODE_TYPE::SPIHT) {
			std::cout << ">>> SPIHT coding begin" << endl;

			bout->put_bits('S', 8);
			bout->put_bits('P', 8);
			bout->put_bits('I', 8);
			bout->put_bits('H', 8);
			bout->put_bits('T', 8);
			bout->put_bits(transType, 6);
			bout->put_bits(levelNum, 6);///levelNum
			bout->put_bits(codeColorType, 6);
			bout->put_bits(maxChannel, 6);
			bout->put_bits(image.cols, 12);
			bout->put_bits(image.rows, 12);
			//bout->put_bits(vectorImages.size(), 12);
			sumInformData += 8 * 5 + 6 * 4 + 12 * 2;
			if (transType == TRANS_TYPE::DWT_CDF97 || transType == TRANS_TYPE::DWT_CDF53) {
				cv::Mat *channel = new cv::Mat[maxChannel];
				cv::Mat input;
				if(transType == TRANS_TYPE::DWT_CDF97) {
					input = cdf97->getForward();
				}
				else {
					input = cdf53->getForward();
				}
				if (maxChannel == 3) {
					split(input, channel);
				}
				else {
					channel[0] = input;
				}

				//std::cout << "channel: " << cdf97->getForward().channels() << endl;
				for (int ch = 0; ch < maxChannel; ch++) {
					ostringstream temp;
					BitOutputStream* spihtOut = new BitOutputStream(&temp);

					Matrix mat = Matrix(channel[ch], 1);
					Matrix absMat = Tool::Abs(mat);
					//int32_t max = absMat.getMax();
					int len = mat.getCountBits();//Tool::DecToBin(max).length();
					int bits0 = (int)ceil(100*8000*0.33);// * (mat.w() * mat.h())
					spihtOut->put_bits(bits0, 29);
					cout << "channel_" << ch << endl;
					SPIHT_Encoder encode;
					if(transType == TRANS_TYPE::DWT_CDF97) {
						encode = SPIHT_Encoder(&mat, cdf97->getLevelsObject(), levelNum, ch);
					}
					else {
						encode = SPIHT_Encoder(&mat, cdf53->getLevelsObject(), levelNum, ch);
					}
					///levelNum
					encode.encode(bits0, spihtOut);
					//std::cout << endl;
					spihtOut->flush_bits();
					spihtOut->flush();
					//string comp = Tool::GetCompressDataString(temp.str());
					/*bout->put_bits(comp.length(), 32);
					for(int l = 0; l < comp.length(); l++) {
						bout->put_bits(comp[l],8);
					}*/
					int lenStream = spihtOut->getActualLen();
					cout << lenStream << endl;
					bout->put_bits(lenStream, 32);
					istringstream is(temp.str());
					BitInputStream* bis = new BitInputStream(&is);
					for (int b = 0; b < lenStream; b++) {
						bout->put_bit(bis->get_bit());
					}

				}
			}
			else if (transType == TRANS_TYPE::CON) {
				for (int ch = 0; ch < contur.size(); ch++) {
					ostringstream temp;
					BitOutputStream* spihtOut = new BitOutputStream(&temp);
					///
					int32_t maxCon = 0;
					//int allWidth = 0, allHeight = 0;
					double min, max;
					std::vector<std::vector<cv::Mat>> high = contur[ch].getHigh();
					cv::Mat low = contur[ch].getLow();
					//allWidth += low.cols;
					//allHeight += low.rows;
					//matrixLow = Matrix(low);
					cv::minMaxLoc(cv::abs(low), &min, &max);
					maxCon = static_cast<int32_t>(max);
					//maxChild = high.size();
					for (int i = 0; i < high.size(); i++) {
						//std::vector<Matrix> temp;
						for (int k = 0; k < high[i].size(); k++) {
							//allWidth += high[i][k].cols;
							//allHeight += high[i][k].rows;
							cv::minMaxLoc(cv::abs(high[i][k]), &min, &max);
							if (maxCon < max) {
								maxCon = static_cast<int32_t>(max);
							}
						}

					}
					int len = Tool::DecToBin(maxCon).length();
					int bits0 = (int)ceil(setValue[1]*8000*0.33);
					spihtOut->put_bits(bits0, 29);
					cout << "channel_" << ch << endl;
					SPIHT_Encoder_Contourlet encode = SPIHT_Encoder_Contourlet(contur[ch], levelNum);///levelNum
					encode.encode(bits0, spihtOut);
					
					spihtOut->flush_bits();
					spihtOut->flush();

					int lenStream = spihtOut->getActualLen();
					bout->put_bits(lenStream, 32);
					istringstream is(temp.str());
					BitInputStream* bis = new BitInputStream(&is);
					for (int b = 0; b < lenStream; b++) {
						bout->put_bit(bis->get_bit());
					}
				}
			}
			bout->flush();
			std::cout << "<<< SPIHT coding done" << endl;
			std::cout << "BitstreamFileSize:............................" << (bout->getActualLen() - sumInformData) / 8000.0 << "KB" << endl;
		}
		else if (codeType == CODE_TYPE::EBCOT) {
			int tempCX = 0;
			int tempD = 0;
			int tempP = 0;
			int size = 0;
			int planes = 0;
			if(set[0]) {
				planes = setValue[0];
			}
			if(set[2]) {
				planes = 0;
			}
			vector<int> maxPlaneVec, minPlaneVec;

			

			//std::cout << "<<< EBCOT coding begin" << endl;
			vector<DwtImage*> vectorImages;
			vector<vector<DwtImage*>> temp;
			if (transType == TRANS_TYPE::CON) {
				//for (int i = 0; i < contur.size(); i++) {
				//	vector<DwtImage*> temp = contur[i].getBlocks();
				//	vectorImages.reserve(vectorImages.size() + temp.size()); // preallocate memory
				//	vectorImages.insert(vectorImages.end(), temp.begin(), temp.end());

				//}
				
				for(int i = 0; i < contur.size(); i++) {
					temp.push_back(contur[i].getBlocks(block_size));
				}
				
				for(int ii = 0; ii < temp.size(); ii++) {
					int32_t maxCon = 0;
					double min, max = 0;
					for(int i = 0; i < temp[0].size(); i++) {
						vectorImages.push_back(temp[ii][i]);
						cv::minMaxLoc(cv::abs(temp[ii][i]->getImage()), &min, &max);
						if(maxCon < max) {
							maxCon = static_cast<int32_t>(max);
						}
					}
					maxPlaneVec.push_back(Tool::DecToBin(maxCon).length() - 1);
					minPlaneVec.push_back(maxPlaneVec[ii] - planes);
				}
				vectorImages.clear();
				for(int i = 0; i < temp[0].size(); i++) {
					for(int ii = 0; ii < temp.size(); ii++) {
						vectorImages.push_back(temp[ii][i]);
						}
					}
				
			}
			else {
				//cout << "sem" << endl;
				//for(int in =0; in < ebcotTiles[0].size(); in++){
				//	for(int out = 0; out < ebcotTiles.size();out++){
				//		vectorImages.push_back(ebcotTiles[out][in]);
				//	//cout << ebcotTiles[out][in]->w() << "---" << ebcotTiles[out][in]->h() << endl;
				//	}
				//}
				if(transType == TRANS_TYPE::DWT_CDF97) {
					vectorImages = cdf97->getLevelsObject()->getImagesVec3();
				}
				else {
					vectorImages = cdf53->getLevelsObject()->getImagesVec3();
				}
				
				for(int ch = 0; ch< maxChannel; ch++) {
					int32_t maxCon = 0;
					double min, max = 0;
					for(int i = 0; i < vectorImages.size(); i++) {
						cv::minMaxLoc(cv::abs(vectorImages[i]->getImage()), &min, &max);
						if(maxCon < max && vectorImages[i]->getChannel() == ch) {
							maxCon = static_cast<int32_t>(max);
						}
					}
					maxPlaneVec.push_back(Tool::DecToBin(maxCon).length() - 1);
					minPlaneVec.push_back(maxPlaneVec[ch] - planes);
				}
				//cout << "sem" << endl;
			}

			//cout << "after blocks" << endl;
			
			
			
			

			bout->put_bits('E', 8);
			bout->put_bits('B', 8);
			bout->put_bits('C', 8);
			bout->put_bits('O', 8);
			bout->put_bits('T', 8);
			bout->put_bits(transType, 6);
			bout->put_bits(levelNum, 6);///levelNum
			bout->put_bits(codeColorType, 6);
			bout->put_bits(maxChannel, 6);
			bout->put_bits(image.cols, 12);
			bout->put_bits(image.rows, 12);
			bout->put_bits(vectorImages.size(), 12);
			bout->put_bits(block_size, 12);
			//bout->put_bits(kilobytes, 12);
			//bout->put_bits(tilesSize, 12);

			if (transType == TRANS_TYPE::CON) {
				bout->put_bits(contur[0].getLow().cols, 10);
				bout->put_bits(contur[0].getLow().rows, 10);

				bout->put_bits(contur[0].getHigh()[0][0].cols, 10);
				bout->put_bits(contur[0].getHigh()[0][0].rows, 10);

				bout->put_bits(contur[0].getHigh().size(), 10);
				bout->put_bits(contur[0].getHigh()[0].size(), 10);

				sumInformData += 60;
			}

			sumInformData += 8 * 6 + 6 * 4 + 12 * 2;

			//cout << ">>>>>> EBCOT coding begin" << endl;
			//cout << vectorImages.size() << endl;

			int bits = (int)(setValue[1] * 8000);
			//cout << bitstiles << endl;
			std::vector<EBCOT_info> last_info;
			std::vector<EBCOT_info> prev_info;
			int min_plane = 0;
			while(1){
				prev_info = last_info;
				last_info.clear();
					for (int i = 0; i < vectorImages.size(); i++) {//vectorImages.size()
						//cout << vectorImages[i]->getChannel() << endl;
						/*if (i != 0) {
							cout << "=";
						}*/
						if(transType == DWT_CDF97 || transType == DWT_CDF53) {
							min_plane = minPlaneVec[vectorImages[i]->getChannel()];
						}
						else if(transType == CON) {
							if(maxChannel == 3) {
								//int third = vectorImages.size()/3;
								min_plane = minPlaneVec[i%3];
							}
							else {
								min_plane = minPlaneVec[0];
							}
						}
						

						if(min_plane < 0) {
							min_plane=0;
						}

						Matrix ma = Matrix(vectorImages[i]->getImage());
						MyVector2i myvec(vectorImages[i]->w(), vectorImages[i]->h());
						Block_Encoder<int32_t>* block = new Block_Encoder<int32_t>(myvec);
						int plane = Tool::DecToBin(Tool::Abs(ma).getMax()).length()-1;
					
						
				
						block->encode(ma,(block_coder::subband_t)vectorImages[i]->getTypeSubBands(), plane, min_plane);
						vector<uint8_t>  temp = block->get_buffer();
						vector<uint8_t> buffer(temp.begin(),temp.begin()+ block->get_buffer_size());
						last_info.push_back(EBCOT_info(vectorImages[i]->w(), vectorImages[i]->h(), vectorImages[i]->getLevel(), vectorImages[i]->getChannel(), (int)vectorImages[i]->getTypeSubBands(),plane, min_plane,buffer));
						delete block;
				 
					}
					if(set[0] || set[2]) {
						break;
					}
					if(set[1]){
						int countBits=0;
						for(int i = 0; i < last_info.size(); i++) {
							countBits += last_info[i].buffer_size*8;
						}
						if(countBits > bits) {
							break;
						}
						else {
							//min_plane--;
							for(int i = 0; i < maxChannel; i++) {
								minPlaneVec[i]= minPlaneVec[i]-1;
							}
						}
						int testZero = 0;
						for(int i = 0; i < maxChannel; i++) {
							if(minPlaneVec[i] < 0){
								testZero++;
							}
						}
						if(testZero == maxChannel) {
							prev_info = last_info;
							break;
						}
					}
			}
			for(int i = 0; i < prev_info.size(); i++) {
				EBCOT_info in = prev_info[i];
				bout->put_bits(in.width, 10);
				bout->put_bits(in.height, 10);
				bout->put_bits(in.level, 8);
				bout->put_bits(in.channel, 6);
				bout->put_bits(in.band, 6);
				bout->put_bits(in.plane,10);
				bout->put_bits(in.min, 10);
				bout->put_bits(in.buffer_size,32);
				for(int b = 0; b < in.buffer_size; b++) {
					bout->put_bits(in.buffer[b],8);
				}
			}
			cout << endl;
			

			//cout << "<<<<<< EBCOT coding done!" << endl;
			bout->flush();
			std::cout << "BitstreamFileSize:............................" << (bout->getActualLen() - sumInformData) / 8000.0 << "KB" << endl;
		}


	}




	cv::Mat decompresFile( string inputSource,  string outputSource) {
		cv::Mat result;

		FileInputStream* fin = new FileInputStream(inputSource.c_str());
		BitInputStream* bin = new BitInputStream(fin);


		string code;
		for (int i = 0; i < 5; i++) {
			code.push_back((char)bin->get_bits(8));
		}

		cout << code << endl;
		//cout << "lycan" << endl;
		int trans_type = bin->get_bits(6);
		int numLevel = bin->get_bits(6);
		COLOR_SPACE_TYPE codeColorType = (COLOR_SPACE_TYPE)bin->get_bits(6);
		int numChannel = bin->get_bits(6);
		int fullWidth = bin->get_bits(12);
		int fullHeight = bin->get_bits(12);

		std::cout << "Decomress" << endl;

		if (code == "_EZW_" && (trans_type == (int)TRANS_TYPE::DWT_CDF97 || trans_type == (int)TRANS_TYPE::DWT_CDF53)) {
			std::cout << ">>> EZW decoding begin" << endl;
			cv::Mat *channel = new cv::Mat[numChannel];
			for (int ch = 0; ch < numChannel; ch++) {
				deque<unsigned> symbols;
				ostringstream temp1, temp2;
				BitOutputStream* ezwOut1 = new BitOutputStream(&temp1);
				BitOutputStream* ezwOut2 = new BitOutputStream(&temp2);

				int min_threshold= bin->get_bits( 12);
				int len1 = bin->get_bits(32);
				int len2 = bin->get_bits(32);
				unsigned numCodingSymbols = bin->get_bits(8);
				int size = bin->get_bits(32);
				//std::cout << "size: " << size << endl;
				int width = bin->get_bits(12);
				int height = bin->get_bits(12);
				int32_t threshold = bin->get_bits(32);


				for (int b = 0; b < len1; b++) {
					//bitstream.push_back(bin->get_bit());
					ezwOut1->put_bit(bin->get_bit());
				}
				//std::cout << "sem" << endl;
				for (int b = 0; b < len2; b++) {
					ezwOut2->put_bit(bin->get_bit());
				}
				//ezwOut->flush();
				istringstream is1(temp1.str());
				istringstream is2(temp2.str());
				BitInputStream* input1 = new BitInputStream(&is1);
				BitInputStream* input2 = new BitInputStream(&is2);
				//std::cout << "tadyhle" << endl;
				auto ad = ARIDecode(input2, numCodingSymbols);
				//std::cout << "tadyhle" << endl;
				for (size_t s = 0; s < size; ++s) {
					auto decoded = ad.decode();
					symbols.push_back(decoded);
				}

				//cout << "symbols: " << symbols.size() << "_" << symbols[100] << endl;

				EZWDecode* decode = new EZWDecode(symbols, input1);
				Matrix decodeMat = Matrix(width, height);

				decode->decode(threshold, min_threshold, decodeMat);

				channel[ch] = decodeMat.getCvMatDequant(1).clone();

			}
			std::cout << "<<< EZW decoding done!" << endl;
			if (numChannel == 3) {
				cv::Mat test;
				merge(channel, numChannel, test);
				result = test.clone();
				/*test.convertTo(test, CV_8UC1);
				imshow("EZW", test);
				cv::waitKey();*/
			}
			else {
				result = channel[0].clone();
				/*cv::Mat test;
				result.convertTo(test, CV_8UC1);
				imshow("EZW", test);
				cv::waitKey();*/
			}

		}
		else if (code == "SPIHT") {
			std::cout << ">>> SPIHT decoding begin" << endl;
			if (trans_type == (int)TRANS_TYPE::DWT_CDF97 || trans_type == (int)TRANS_TYPE::DWT_CDF53) {
				cv::Mat *channel = new cv::Mat[numChannel];
				for (int ch = 0; ch < numChannel; ch++) {
					int len = bin->get_bits(32);
					cout << len << endl; 
					ostringstream temp;
					BitOutputStream* spihtOut = new BitOutputStream(&temp);
					/*string comp;
					for (int b = 0; b < len; b++) {
						comp.push_back(bin->get_bits(8));
					}*/

					//string temp = Tool::GetDecompressDataString(comp);
					for(int b = 0; b < len; b++) {
						spihtOut->put_bit(bin->get_bit());
					}
					istringstream is(temp.str());
					BitInputStream* input = new BitInputStream(&is);

					Matrix decodeMat = Matrix(fullWidth, fullHeight);
					cout << "channel_" << ch << endl;
					SPIHT_Decoder* spiht_dec = new SPIHT_Decoder(&decodeMat, numLevel, fullWidth, fullHeight);
					int bits0 = input->get_bits(29);
					spiht_dec->decode(0, bits0, input);
					cout << endl;
					channel[ch] = spiht_dec->getOutput(512).getCvMat();// decodeMat.getCvMatDequant(1000).clone();
				}
				std::cout << "<<< SPIHT decoding done!" << endl;
				if (numChannel == 3) {
					cv::Mat test;
					merge(channel, numChannel, test);
					result = test.clone();
					/*test.convertTo(test, CV_8UC1);
					imshow("SPIHT", test);*/
					//cv::waitKey();
				}
				else {
					result = channel[0].clone();
				}
			}
			else if (trans_type == (int)TRANS_TYPE::CON) {
				cv::Mat *channel = new cv::Mat[numChannel];
				for (int ch = 0; ch < numChannel; ch++) {
					int len = bin->get_bits(32);
					ostringstream temp;
					BitOutputStream* spihtOut = new BitOutputStream(&temp);

					for (int b = 0; b < len; b++) {
						spihtOut->put_bit(bin->get_bit());
					}
					istringstream is(temp.str());
					BitInputStream* input = new BitInputStream(&is);

					//Matrix decodeMat = Matrix(fullWidth, fullHeight);
					cout << "channel_" << ch << endl;
					SPIHT_Decoder_Contourlet* spiht_dec = new SPIHT_Decoder_Contourlet();
					int bits0 = input->get_bits(29);
					spiht_dec->decode(0, bits0, input);
					cout << endl;
					MyContourlet mycon = spiht_dec->getOutput();

					mat dest2 = mat_new(fullWidth, fullHeight);;
					contourlet_t *contourlet2;
					//cout << "sekkkk" << endl;
					contourlet2 = mycon.getStruct();
					//cout << "sekkkk" << endl;
					contourlet_itransform(contourlet2, dest2);
					//cout << "sekkkk" << endl;
					channel[ch] = saveMat(dest2);
					contourlet_delete(contourlet2);
					mat_delete(dest2);
				}
				if (numChannel == 3) {
					cv::Mat test;
					merge(channel, numChannel, test);
					result = test.clone();
					/*test.convertTo(test, CV_8UC1);
					imshow("SPIHT", test);*/
					//cv::waitKey();
				}
				else {
					result = channel[0].clone();
				}
				/*result_img.convertTo(result_img, CV_8UC1);
				cv::imshow("SPIHT", result_img);
				cv::waitKey();
				cout << "End spiht" << endl;*/
				if (codeColorType == COLOR_SPACE_TYPE::YCBCR) {
					result = ConvertYCbCr2BGR(result);
				}
				imwrite(outputSource, result);
				return result;

			}

		}
		else if (code == "EBCOT") {
			int numParts = bin->get_bits(12);
			int block_size = bin->get_bits(12);
			//int kilobytes = bin->get_bits(12);
			vector<int> info_con;
			if (trans_type == (int)TRANS_TYPE::CON) {
				for (int i = 0; i < 6; i++) {
					info_con.push_back(bin->get_bits(10));
				}
			}
			
			//std::cout << ">>>>>> EBCOT decoding begin" << endl;
			JoinSubimage* join = new JoinSubimage(fullWidth, fullHeight, numLevel, numChannel, block_size);
		/*	vector<JoinSubimage*> joins;
			for(int i = 0; i < tilesSize; i++) {
				joins.push_back(new JoinSubimage(TILE_SIZE, TILE_SIZE, numLevel, numChannel));
			}*/
			int counter = 0;
			int joinCounter = 0;
			//int bitstiles = (int)kilobytes * 8000 * 0.34;
			for (int i = 0; i < numParts; i++) {
				/*if (i != 0) {
					cout << "=";
				}*/
			

				int width = bin->get_bits(10);
				int height = bin->get_bits(10);
				int level = bin->get_bits(8);
				int channel = bin->get_bits(6);
				int type = bin->get_bits(6);
				int plane = bin->get_bits(10);
				int min = bin->get_bits(10);
				int32_t buffer_size = bin->get_bits(32);
				vector<uint8_t> buffer;
				for(int b=0; b < buffer_size; b++){
					buffer.push_back(bin->get_bits(8));
				}
				
				Matrix ma = Matrix(width,height);
				MyVector2i myvec(width, height);
				block_decoder<int32_t> block = block_decoder<int32_t>(myvec);
				//int plane = Tool::DecToBin(Tool::Abs(ma).getMax()).length() - 1;
				bivector<int32_t> bi = bivector<int32_t>(myvec, 64);
				for(int xx = 0; xx < myvec.x(); xx++) {
					for(int yy = 0; yy < myvec.y(); yy++) {
						bi.at(xx, yy) = 0;
						}
				}
				block.decode(buffer.data(),buffer_size,plane+1,bi,(block_coder::subband_t)type,min);
				//block.encode(bi, (block_coder::subband_t)vectorImages[i]->getTypeSubBands(), plane);
				for(int xx = 0; xx < myvec.x(); xx++) {
					for(int yy = 0; yy < myvec.y(); yy++) {
						ma[cv::Point(xx,yy)] = bi.at(xx, yy) << min;
					}
				}
				
				/*if(true) {
					cout << "out: " << Tool::DecToBin(ma.get(cv::Point(10, 10)))<< endl;
					cout << "out: " << Tool::DecToBin(ma.get(cv::Point(15, 15))) << endl;
					cout << "out: " << Tool::DecToBin(ma.get(cv::Point(16, 16))) << endl;
					cout << "out: " << Tool::DecToBin(ma.get(cv::Point(17, 17))) << endl;
					cout << endl;
				}*/
				/*cv::Mat tmp2;
				ma.getCvMat().convertTo(tmp2, CV_8U);
				imshow("EBCOT", tmp2);
				cv::waitKey();*/
				/*if(i != 0 && i % (numParts / tilesSize) == 0) {
					joinCounter++;
				}*/
				join->add(DwtImage(ma.getCvMat(),level,channel,numLevel,numChannel,(Bands)type));
				
				//cout << joinCounter << endl;
				/*if (i % 40 == 0 && i != 0) {
					cout << endl;
				}*/
				//cout << numParts << endl;

			}
			cout << endl;
			//cout << "po" << endl;
			cv::Mat test;
			if (trans_type == (int)TRANS_TYPE::DWT_CDF97 || trans_type == (int)TRANS_TYPE::DWT_CDF53) {
				test = join->getJoinImage();
			}
			else if (trans_type == (int)TRANS_TYPE::CON) {
				test = join->getJoinImage2(info_con);
				//cout << "join" << endl;
				if (codeColorType == COLOR_SPACE_TYPE::YCBCR) {
					test = ConvertYCbCr2BGR(test);
				}
				imwrite(outputSource, test);
				return test;
			}
			cv::Mat tmp2;
			test.convertTo(tmp2, CV_8U);
			imwrite(outputSource, tmp2);

			/*imshow("EBCOT", tmp2);
			cv::waitKey();*/
			result = test.clone();
			//std::cout << "<<<<<< EBCOT decoding done!" << endl;

		}
		else {
			cerr << "Error with decompress file." << endl;
			exit(1);
		}
		//waitKey();




		//*************************Inverse DWT ****************************************//
		if(trans_type == (int)TRANS_TYPE::DWT_CDF97) {
			//std::cout << ">>> Cdf 9 / 7 begin" << endl;
			if(result.empty()) {
				cerr << "Empty image" << endl;
				}
			//std::cout << result.channels() << endl;
			CDF97 *cdf97 = new CDF97(numLevel, result);
			cdf97->inverse2d(result, 0);
			result = cdf97->getInverse();
			//std::cout << "<<< Cdf 9 / 7 done!" << endl;
			if(codeColorType == COLOR_SPACE_TYPE::YCBCR) {
				result = ConvertYCbCr2BGR(result);
				}
			imwrite(outputSource, result);
			return result;
		}
		else {
			//std::cout << ">>> Cdf 5 / 3 begin" << endl;
			if(result.empty()) {
				cerr << "Empty image" << endl;
				}
			//std::cout << result.channels() << endl;
			CDF53 *cdf53 = new CDF53(numLevel, result);
			cdf53->inverse2d(result, 0);
			result = cdf53->getInverse();
			//std::cout << "<<< Cdf 5 / 3 done!" << endl;
			if(codeColorType == COLOR_SPACE_TYPE::YCBCR) {
				result = ConvertYCbCr2BGR(result);
				}
			imwrite(outputSource, result);
			return result;
		}
		
	
}

