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

class BitInputStream {
	InputStream* in;
	std::istream* inStream;
	int bit_count;
	int buffer;
	bool iStream;
public:
	BitInputStream(InputStream* _in) {
		in = _in;
		bit_count = 0;
		buffer = 0;
		iStream = false;
	};
	BitInputStream(std::istream* _in) {
		inStream = _in;
		bit_count = 0;
		buffer = 0;
		iStream = true;
	};

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

	unsigned long get_bits(const int size) { /* read #size bits from input */
		unsigned long value = 0;
		for(int i = 0; i < size; i++) {
			value <<= 1;
			value |= get_bit();
			}
		return value;
	}   

	};

class BitOutputStream {
	OutputStream* out;
	std::ostream* outStream;
	std::vector<uint8_t> pokus;
	int bit_count;
	int buffer;
	int64_t bitStreamLen;
	bool osStream;
public:
	BitOutputStream(OutputStream* _out) {
		out = _out;
		bit_count = 0;
		buffer = 0;
		bitStreamLen = 0;
		osStream = false;
	}
	BitOutputStream(std::ostream* _out) {
		outStream = _out;
		bit_count = 0;
		buffer = 0;
		bitStreamLen = 0;
		osStream = true;
	}

	int64_t getActualLen() {
		return bitStreamLen;
	}

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
	void put_bits(const unsigned long bits, const int size) {/* write #size bits to output */
		for(int i = size - 1; i >= 0; i--)
			put_bit((bits >> i) & 1);
	}
  
	void flush_bits() {
		while(bit_count > 0)
			put_bit(0);
	}

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

