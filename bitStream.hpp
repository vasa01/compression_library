#pragma once
#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <cstdint>
#include <stdexcept>

//using namespace std;

class BitStreamRead {
public:
	BitStreamRead::BitStreamRead(std::istream* stream):byte(0),mask(0),stream(stream) {
	}
	void BitStreamRead::reset(std::istream* stream){
		this->byte=0;
		this->mask=0;
		this->stream = stream;
	}
	bool BitStreamRead::bitStreamRead() {
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
	BitStreamWrite::BitStreamWrite(std::ostream* stream):byte(0), mask(0x80), stream(stream) {

	}
	
	void BitStreamWrite::reset(std::ostream* stream) {
		this->byte = 0;
		this->mask = 0x80;
		this->stream = stream;
	}

	~BitStreamWrite() {
		flush();
	}

	void BitStreamWrite::flush() {
		if(mask != 0x80) {
			stream->put(byte);
			byte = 0;
			mask = 0x80;
		}
	}

	void BitStreamWrite::writeBit(bool bit) {
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