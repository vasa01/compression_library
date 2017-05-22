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
 * @file	bitIOStream.hpp
 *
 * @brief	bit stream.
 */

#ifndef BITIOSTREAM_H
#define BITIOSTREAM_H


#include "iOStream.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

///using namespace std;
/**
 * class BitInputStream
 */
class BitInputStream {
	InputStream* in;
	std::istream* inStream;
	int bit_count;
	int buffer;
	bool iStream;
public:
	/**
	 * BitInputStream create
	 * @param _in
	 */
	BitInputStream(InputStream* _in) {
		in = _in;
		bit_count = 0;
		buffer = 0;
		iStream = false;
	};
	/**
	 * BitInputStream create
	 * @param _in
	 */
	BitInputStream(std::istream* _in) {
		inStream = _in;
		bit_count = 0;
		buffer = 0;
		iStream = true;
	};

	/**
	 * get_bit
	 * @return  - bit
	 */
	int get_bit() {
		if(bit_count == 0) {
			if (iStream) {
				inStream->read(reinterpret_cast<char*>(&buffer), 1);
			}else{
				buffer = in->get_char();
			}
			
			if(buffer == EOF)
				return 0;
			else
				bit_count = 8;
		}
		int bit = (buffer >> --bit_count) & 1;
		//cout << "i "<<bit << endl;
		return bit;
	}

	/**
	 * get_bits
	 * @param size  - size of read bits
	 * @return - bits number
	 */
	unsigned long get_bits(const int size) { /* read #size bits from input */
		unsigned long value = 0;
		for(int i = 0; i < size; i++) {
			value <<= 1;
			value |= get_bit();
			}
		return value;
	}   

	};
/**
 * class BitOutputStream
 */
class BitOutputStream {
	OutputStream* out;
	std::ostream* outStream;
	std::vector<uint8_t> pokus;
	int bit_count;
	int buffer;
	int64_t bitStreamLen;
	bool osStream;
public:
	/**
	 * BitOutputStream
	 * @param _out
	 */
	BitOutputStream(OutputStream* _out) {
		out = _out;
		bit_count = 0;
		buffer = 0;
		bitStreamLen = 0;
		osStream = false;
	}
	/**
	 * BitOutputStream
	 * @param _out
	 */
	BitOutputStream(std::ostream* _out) {
		outStream = _out;
		bit_count = 0;
		buffer = 0;
		bitStreamLen = 0;
		osStream = true;
	}

	/**
	 * getActualLen
	 * @return - buffer size
	 */
	int64_t getActualLen() {
		return bitStreamLen;
	}

	/**
	 * put_bit
	 * @param bit
	 */
	void put_bit(int bit) {
		//cout << "i " << bit << " bit_count " << bit_count << endl;
		bitStreamLen++;
		buffer <<= 1;
		buffer |= bit;
		if(++bit_count == 8) {
			pokus.push_back(buffer);
			if (osStream) {
				outStream->put(buffer);
			}
			else {
				out->put_char(buffer);
			}
			bit_count = 0;
			buffer = 0;
		}
	}
	/**
	 * put_bits
	 * @param bits
	 * @param size
	 */
	void put_bits(const unsigned long bits, const int size) {/* write #size bits to output */
		for(int i = size - 1; i >= 0; i--)
			put_bit((bits >> i) & 1);
	}

	/**
	 * flush_bits
	 */
	void flush_bits() {
		while(bit_count > 0)
			put_bit(0);
	}

	/**
	 * flush
	 */
	void flush() { 
		flush_bits(); 
		if (osStream) {
			outStream->flush();
		}
		else {
			//std::ofstream File;
			//File.open("file.txt", std::ios::out | std::ios::binary);
			////File.write(pokus.c_str(), pokus.length());
			//File.close();
			/*std::ofstream output_file("./example.txt");
			std::ostream_iterator<std::uint8_t> output_iterator(output_file, "\n");
			std::copy(pokus.begin(), pokus.end(), output_iterator);*/
			out->flush();
			
		}
	}
		
	};

#endif

