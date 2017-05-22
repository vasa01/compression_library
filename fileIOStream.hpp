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
 * @file	fileIOStream.hpp
 *
 * @brief	fileIOStream implementation.
 */

#ifndef FILEIOSTREAM_H
#define FILEIOSTREAM_H

#pragma warning(disable:4996)

#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <cstdio>

#include "iOStream.hpp"
//#include "tool.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>

/**
 * class FileInputStream
 */
class FileInputStream: public InputStream {
	static const long BUF_SIZE = 65535;    /* buffer size */
	char* buffer;
	long buf_pos;
	long buf_size;                         /* real buffer size */
	FILE* fp;
public:
	/**
	 * create FileInputStream
	 * @param fn
	 */
	FileInputStream(const char* fn) { /* fn = input file name */
		buffer = new char[BUF_SIZE];
		if(buffer == NULL)
			std::cerr << "Exception::ERR_MEMORY" << std::endl;
		buf_pos = 0;
		buf_size = 0;
		fp = fopen(fn, "rb");
		if(fp == NULL)
			std::cerr << "Exception::ERR_FILE_NOT_FOUND" << std::endl;
	};       

	~FileInputStream() {
		fclose(fp);
	}

	/**
	 * get char
	 * @return - char
	 */
	int get_char(){
		if(buf_pos > 0 && buf_size == 0)
			return EOF;
		if(buf_pos == buf_size) {
			buf_size = (long)fread((void*)buffer, sizeof(char), BUF_SIZE, fp);
			if(buf_size == 0)
				return EOF;
			buf_pos = 0;
			}
		return (int)((unsigned char)buffer[buf_pos++]);
	}

	/**
	 * get size
	 * @return  - return size
	 */
	long get_size() {
		long cur_pos = ftell(fp);
		fseek(fp, 0, SEEK_END);
		long result = ftell(fp);
		fseek(fp, cur_pos, SEEK_SET);
		return result;
	}
	
};

/**
 * class FileOutputStream
 */
class FileOutputStream: public OutputStream {
	static const long BUF_SIZE = 65535;    /* buffer size */
	char* buffer;
	//string temp;
	long buf_pos;
	FILE* fp;
	//ifstream file;
public:
	/**
	 * create FileOutputStream
	 * @param fn
	 */
	FileOutputStream(const char* fn) {
		buffer = new char[BUF_SIZE];
		if(buffer == NULL)
			std::cerr << "Exception::ERR_MEMORY" <<std::endl;
		buf_pos = 0;
		fp = fopen(fn, "wb");
		//file = ifstream("binnnn", ios::in | ios::binary | ios::ate);
		if(fp == NULL)
			std::cerr <<"Exception::ERR_FILE_ACCESS"<<std::endl;
	}
	
	~FileOutputStream() {
		flush();
		fclose(fp);
	}

	/**
	 * put_char c
	 * @param c
	 */
	void put_char(int c) {
		if(buf_pos == BUF_SIZE) {
			if((long)fwrite((void*)buffer, sizeof(char), BUF_SIZE, fp) != BUF_SIZE)
				std::cerr << "Exception::ERR_FILE_WRITE" << std::endl;
			buf_pos = 0;
			}
		buffer[buf_pos++] = (char)c;
		//temp.push_back((char)c);
	}
	/**
	 * flush
	 */
	void flush() {
		//fwrite(buffer, sizeof(buffer[0]), sizeof(buffer) / sizeof(buffer[0]), fp);
		if(buf_pos > 0 && fwrite((void*)buffer, sizeof(char), buf_pos, fp) != buf_pos)
			std::cerr << "Exception::ERR_FILE_WRITE" << std::endl;
		buf_pos = 0;
		//ofstream fout("binka", ios::binary);
		//fout << temp;
		fflush(fp);
	}

	};

#endif