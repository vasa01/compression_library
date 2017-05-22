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
 * @file	main,cpp
 *
 * @brief	Main app to control library implementation.
 */

//#define _GLIBCXX_USE_CXX11_ABI 0

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





/**
 * Main function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]){

	std::string imgSource, outSource;
	int levels = 0;
	int planes = 5;
	int trans = 0;
	int color = 0;
	int bytes = 0;
	int encode = 0;
	int quant = 1;
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
		else if(std::string(argv[i]) == "-q" && i + 1 < argc) {
			quant = atoi(argv[++i]);
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
            		    std::cout << "###############################################################################" << std::endl;
			    std::cout << "# Help  - compression library                                                 #" << std::endl;
			    std::cout << "###############################################################################" << std::endl;
			    std::cout << "# -i  source - set input source image                                         #" << std::endl;
			    std::cout << "# -o  source - set output source image                                        #" << std::endl;
			    std::cout << "# -t  number - set transform type                                             #" << std::endl;
			    std::cout << "#         - 0 is DWT_CDF9/7, 1 is DWT_CDF5/3, 2 is contoulet                  #" << std::endl;
			    std::cout << "# -c  number - set color space type                                           #" << std::endl;
			    std::cout << "#         - 0 is YCBCR, 1 is RGB, 2 is  GRAY                                  #" << std::endl;
			    std::cout << "# -e  number - set encode coef type                                           #" << std::endl;
			    std::cout << "#         - 0 is EZW, 1 is SPIHT, 2 is EBCOT                                  #" << std::endl;
			    std::cout << "# -l  number - set level number in transform                                  #" << std::endl;
			    std::cout << "# -s  number - set number maxsize compresss file in kB                        #" << std::endl;
			    std::cout << "# -p  number - set number maxplanes in encoding coef                          #" << std::endl;
			    std::cout << "# -b  number - set number block size in encoding coef EBCOT (defaul 64)       #" << std::endl;
            		    std::cout << "# -q  number - if number == 0 quantization turn off else turn on   		#" << std::endl;
            		    std::cout << "###############################################################################" << std::endl;
			return 0;
		}
		else {
			std::cerr << "Error: Doesn't mean parametr \"" << argv[i] << "\". Use -h for more information." << std::endl;
			exit(-1);
		}
	}
	if(imgSource == "" || outSource == "") {
		std::cerr << "Error: No input or output source code in param." << std::endl;
		exit(-1);
	}

	if(bitPlaneTest && byteTest) {
		std::cerr << "Error: Unsupported combination of parameter -s a -p. Only one can be." << std::endl;
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
		std::cerr << "Error: Unsupported combination of transformation and encoding." << std::endl;
		exit(-1);
	}

	if(code_type == COMPR_LBR::CODE_TYPE::SPIHT && bitPlaneTest) {
		std::cerr << "Error: Unsupported combination of parameter -e 1 and -p, Use -s for size." << std::endl;
		exit(-1);
	}

	if(code_type == COMPR_LBR::CODE_TYPE::EZW && byteTest) {
		std::cerr << "Error: Unsupported combination of parameter -e 0 and -s, Use -p to indicate the number of bit planes." << std::endl;
		exit(-1);
	}

	if(code_type != COMPR_LBR::CODE_TYPE::EBCOT && maxTest) {
		std::cerr << "Enter the -s or -p parameter to specify the size of the compressed file." << std::endl;
		exit(-1);
	}

	if(block_size < 1) {
		std::cerr << "Error: The size of the EBCOT block must not be zero." << std::endl;
		exit(-1);
	}
	
	

	vector<bool> paramTest;
	paramTest.push_back(bitPlaneTest);
	paramTest.push_back(byteTest);
	paramTest.push_back(maxTest);

	vector<int> values;
	values.push_back(planes);
	values.push_back(bytes);


	compresFile(imgSource, "test", color_type, trans_type, level, code_type , paramTest, values, block_size, quant);


	cv::Mat test = decompresFile("test", outSource);


	PSNR psnr = PSNR(imgSource, outSource);
	std::cout << "PSNR: " << psnr.getPSNR() << endl;

	SSIM ssim = SSIM(imgSource, outSource);
	std::cout << "SSIM: " << ssim.get2() << " SSIM: " << ssim.getMSSIM() << endl;

		
}
