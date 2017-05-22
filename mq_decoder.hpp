#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "predicter.hpp"

class Decoder {
private:
	const char *encoded;
	size_t encoded_length;
	size_t encoded_pos;
	char *decoded;
	size_t decoded_length;
	size_t decoded_pos;

	uint32_t range;
	uint32_t value;

	bool printDebug;

	void WriteBit(int bit) {
		assert(bit == 0 || bit == 1);

		if(decoded_pos >= decoded_length * 8) {
			char *new_decoded = new char[this->decoded_length * 2];
			for(size_t i = 0; i < this->decoded_length; ++i) {
				new_decoded[i] = this->decoded[i];
				}
			for(size_t i = this->decoded_length; i < this->decoded_length * 2; ++i) {
				new_decoded[i] = 0;
				}
			delete[] this->decoded;
			this->decoded = new_decoded;
			this->decoded_length *= 2;
			}

		this->decoded[this->decoded_pos / 8] |= bit << (this->decoded_pos % 8);
		this->decoded_pos += 1;
		}

	int ReadBit() {
		if(this->encoded_pos < this->encoded_length * 8) {
			char byte = this->encoded[this->encoded_pos / 8];
			int bit = 0;
			if((byte & (1 << (this->encoded_pos % 8)))) {
				bit = 1;
				}
			this->encoded_pos++;
			return bit;
			}
		printf("Reading zero\n");
		return 0;
		}

	int DecodeBit(uint64_t probability) {
		assert(this->value < this->range);
		while(this->range < 0x80000000U) {
			this->range <<= 1;
			this->value <<= 1;
			this->value += this->ReadBit();
			}

		uint32_t split = this->range - ((this->range * probability) >> 32);
		assert(split < this->range);
		if(this->printDebug)
			printf("prob=%08lx range=%08x value=%08x split=%08x\n", probability, this->range, this->value, split);

		if(this->value >= split) {
			this->value -= split;
			this->range -= split;
			//if (this->printDebug) printf("Read 1\n");
			return 1;
			}
		else {
			this->range = split;
			//if (this->printDebug) printf("Read 0\n");
			return 0;
			}
		}

public:
	Decoder()
		: encoded(0), decoded(0), printDebug(false)
		{
		}

	~Decoder() {
		delete[] decoded;
		}

	void Decode(const char *data, size_t len, size_t out_len) {
		this->encoded = data;
		this->encoded_length = len;
		this->encoded_pos = 0;
		delete[] decoded;
		this->decoded = new char[16];
		for(int i = 0; i < 16; ++i) this->decoded[i] = 0;
		this->decoded_length = 16;
		this->decoded_pos = 0;
		this->range = 1;
		this->value = 0;

		Predicter pred;
		while(this->decoded_pos < out_len * 8) {
			//this->printDebug = (this->decoded_pos >= (out_len - 2) * 8);
			uint32_t prob = pred.Probability();
			int bit = this->DecodeBit(prob);
			this->WriteBit(bit);
			pred.Update(bit, prob);
			}
		//pred.PrintStatus();
		}

	const char *GetDecoded() {
		while(this->decoded_pos % 8) {
			this->WriteBit(0);
			}
		return this->decoded;
		}

	int GetDecodedLength() {
		while(this->decoded_pos % 8) {
			this->WriteBit(0);
			}
		return this->decoded_pos / 8;
		}
	};

#endif