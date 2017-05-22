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

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <cstdint>
#include <stdexcept>

//using namespace std;
/**
 * class BitStreamRead
 */
class BitStreamRead {
public:
	BitStreamRead(std::istream* stream):byte(0),mask(0),stream(stream) {
	}
	/**
	 * reset
	 * @param stream
	 */
	void reset(std::istream* stream){
		this->byte=0;
		this->mask=0;
		this->stream = stream;
	}
	/**
	 * bitStreamRead
	 * @return
	 */
	bool bitStreamRead() {
		if(mask == 0) {
			if(!stream->read(reinterpret_cast<char*>(&byte), 1))
				throw std::runtime_error("Unable to read from stream!");
			mask = 0x80;
			}

		bool bit = !!(byte & mask);
		mask >>= 1;
		return bit;
	}
private:
	std::istream* stream;
	uint8_t byte;
	uint8_t mask;
};

class BitStreamWrite {
public:
	BitStreamWrite(std::ostream* stream):byte(0), mask(0x80), stream(stream) {

	}
	
	void reset(std::ostream* stream) {
		this->byte = 0;
		this->mask = 0x80;
		this->stream = stream;
	}

	~BitStreamWrite() {
		flush();
	}

	void flush() {
		if(mask != 0x80) {
			stream->put(byte);
			byte = 0;
			mask = 0x80;
		}
	}

	void writeBit(bool bit) {
		// set bit if we should
		if(bit)
			byte |= mask;

		mask >>= 1;
		if(mask == 0) {
			if(!stream->put(byte))
				throw std::runtime_error("Unable to put byte into stream!");

			byte = 0;
			mask = 0x80;
		}
	}

private:
	std::ostream* stream;

	uint8_t byte;
	uint8_t mask;
};

#endif
