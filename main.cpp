/**********************************************************************************************//**
 * @file	main.cpp
 *
 * @brief	Implements the main class.
 **************************************************************************************************/

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

#include <random>
#include <vector>
#include <algorithm>
#include "compressionLibrary.hpp"

#include "ssim.hpp"
#include "psnr.hpp"




/**********************************************************************************************//**
 * @fn	int main(int argc, char* argv[])
 *
 * @brief	Main entry-point for this application.
 *
 * @author	Vasa 0
 * @date	06.01.2017
 *
 * @param	argc	The number of command-line arguments provided.
 * @param	argv	An array of command-line argument strings.
 *
 * @return	Exit-code for the process - 0 for success, else an error code.
 **************************************************************************************************/

int main(int argc, char* argv[]){

	std::string imgSource, outSource;
	int levels = 0;
	int planes = 5;
	int trans = 0;
	int color = 0;
	int bytes = 0;
	int encode = 0;
	int block_size = 64;
	bool bitPlaneTest = false;
	bool byteTest = false;
	bool maxTest = false;
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "-i" && i + 1 < argc) {
			imgSource = argv[++i];
		}else if(std::string(argv[i]) == "-o" && i + 1 < argc) {
			outSource = argv[++i];
		}
		else if (std::string(argv[i]) == "-l" && i + 1 < argc) {
			levels = atoi(argv[++i]);
		}
		else if(std::string(argv[i]) == "-t" && i + 1 < argc) {
			trans = atoi(argv[++i]);
		}
		else if(std::string(argv[i]) == "-c" && i + 1 < argc) {
			color = atoi(argv[++i]);
		}
		else if(std::string(argv[i]) == "-e" && i + 1 < argc) {
			encode = atoi(argv[++i]);
		}
		else if(std::string(argv[i]) == "-b" && i + 1 < argc) {
			block_size = atoi(argv[++i]);
		}
		else if(std::string(argv[i]) == "-s" && i + 1 < argc) {
			bytes = atoi(argv[++i]);
			byteTest = true;
		}
		else if(std::string(argv[i]) == "-p" && i + 1 < argc) {
			planes = atoi(argv[++i]);
			bitPlaneTest = true;
		}
		else if (std::string(argv[i]) == "-h") {
			std::cout << "Use: " << argv[0] << " -i inputImage -n levels" << std::endl;
			return 0;
		}
		else {
			std::cerr << "Error: Neznamy parametr \"" << argv[i] << "\" pouzijte -h pro vice informaci." << std::endl;
			exit(-1);
		}
	}
	if(imgSource == "" || outSource == "") {
		std::cerr << "Chyba: Neni zadan vstupni nebo vystupni soubor." << std::endl;
		exit(-1);
	}

	if(bitPlaneTest && byteTest) {
		std::cerr << "Chyba: Nepodporovaná kombinace parametru -s a -p. Muze byt pouze jeden." << std::endl;
		exit(-1);
	}

	if(!bitPlaneTest && !byteTest) {
		maxTest = true;
	}

	COMPR_LBR::COLOR_SPACE_TYPE color_type = (COMPR_LBR::COLOR_SPACE_TYPE)color;
	COMPR_LBR::TRANS_TYPE trans_type = (COMPR_LBR::TRANS_TYPE)trans;
	COMPR_LBR::LEVEL_NUM level = (COMPR_LBR::LEVEL_NUM)levels;
	COMPR_LBR::CODE_TYPE code_type = (COMPR_LBR::CODE_TYPE)encode;

	if(code_type == COMPR_LBR::CODE_TYPE::EZW && trans_type == COMPR_LBR::TRANS_TYPE::CON) {
		std::cerr << "Chyba: Nepodporovaná kombinace transformace a kódování." << std::endl;
		exit(-1);
	}

	if(code_type == COMPR_LBR::CODE_TYPE::SPIHT && bitPlaneTest) {
		std::cerr << "Chyba: Nepodporovaná kombinace SPIHT a parametr -p, vyuzijte parametru -s pro udani velikosti." << std::endl;
		exit(-1);
	}

	if(code_type == COMPR_LBR::CODE_TYPE::EZW && byteTest) {
		std::cerr << "Chyba: Nepodporovaná kombinace EZW a parametr -s, vyuzijte parametru -p pro udani poctu bitovych rovin, ktere se maji zpracovat." << std::endl;
		exit(-1);
	}

	if(code_type != COMPR_LBR::CODE_TYPE::EBCOT && maxTest) {
		std::cerr << "Chyba: Zadejte parametr -s nebo -p pro udani velikosti komprimovaneho souboru." << std::endl;
		exit(-1);
	}

	if(block_size < 1) {
		std::cerr << "Chyba: Velikost EBCOT bloku musí být vìtší než nula." << std::endl;
		exit(-1);
	}
	
	vector<bool> paramTest;
	paramTest.push_back(bitPlaneTest);
	paramTest.push_back(byteTest);
	paramTest.push_back(maxTest);

	vector<int> values;
	values.push_back(planes);
	values.push_back(bytes);
	//double min_size = 5;
	//int max_size = 80;
	//int step = 1;
	//while(min_size < max_size){
		compresFile(imgSource, "test", color_type, trans_type, level, code_type , paramTest, values, block_size);


		cv::Mat test = decompresFile("test", "decode.bmp");

		//cv::imwrite("decode_EBCOT_con_rentgen_bitplane_YCbCr_"+Tool::IntToString(min_size)+".bmp",test);

		PSNR psnr = PSNR(imgSource, "decode.bmp");
		std::cout << "PSNR: " << psnr.getPSNR() << endl;

		SSIM ssim = SSIM(imgSource, "decode.bmp");
		std::cout << "SSIM: " << ssim.get2() << " SSIM: " << ssim.getMSSIM() << endl;
		/*if(min_size > 100) {
			step = 50;
		}
		if(min_size > 200) {
			step = 100;
		}*/
		//min_size +=step; 
	//}

		
}