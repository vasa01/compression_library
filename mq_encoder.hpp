#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "predicter.hpp"

class Encoder {
private:
	uint64_t limitLow;
	uint64_t limitHigh;
	char *encoded;
	int encode_pos;
	int encode_length;
	bool finished;
	int limitHighBit;
	bool printDebug;

	void WriteBit(int bit) {
		assert(bit == 0 || bit == 1);

		if(this->encode_pos >= this->encode_length * 8) {
			char *new_encoded = new char[this->encode_length * 2];
			for(int i = 0; i < this->encode_length; ++i) {
				new_encoded[i] = this->encoded[i];
				}
			for(int i = this->encode_length; i < this->encode_length * 2; ++i) {
				new_encoded[i] = 0;
				}
			delete[] this->encoded;
			this->encoded = new_encoded;
			this->encode_length *= 2;
			}

		this->encoded[this->encode_pos / 8] |= bit << (this->encode_pos % 8);
		this->encode_pos += 1;
		}

	void EncodeBit(int nextBit, uint64_t probability) {
		//if (this->printDebug) printf("Writing %d\n", nextBit);
		assert(this->limitHigh - this->limitLow >= 0x80000000U);

		uint64_t split = this->limitHigh -
			(((this->limitHigh - this->limitLow) * probability) >> 32);
		assert(split > this->limitLow && split < this->limitHigh);

		//printf("high %08lx, low %08lx\n", limitHigh, limitLow);
		if(this->printDebug) {
			printf("prob=%08lx range=%08lx split=%08lx low=%08lx high=%08lx\n", probability, limitHigh - limitLow, split - limitLow, limitLow, limitHigh);
			}

		if(nextBit == 0) {
			limitHigh = split;
			}
		else {
			limitLow = split;
			}

		while((limitLow & (1ULL << this->limitHighBit)) ==
			(limitHigh & (1ULL << this->limitHighBit))) {
			WriteBit((limitLow & (1ULL << this->limitHighBit)) ? 1 : 0);
			this->limitHighBit--;
			limitLow &= (1ULL << (this->limitHighBit + 1ULL)) - 1ULL;
			limitHigh &= (1ULL << (this->limitHighBit + 1ULL)) - 1ULL;
			}

		while(this->limitHigh - this->limitLow < 0x80000000U) {
			this->limitLow <<= 1;
			this->limitHigh <<= 1;
			this->limitHighBit++;
			}
		if(this->limitHighBit >= 60) printf("High bit %02d\n", this->limitHighBit);
		}

	void FinishEncoding() {
		if(!this->finished) {
			while(this->encode_pos % 8) {
				this->WriteBit(0);
				}
			this->finished = true;
			}
		}

public:
	Encoder()
		: encoded(0), printDebug(false)
		{
		}

	~Encoder() {
		delete[] encoded;
		}

	void Encode(const char *data, int length) {
		this->limitLow = 0;
		this->limitHigh = 0x100000000ULL;
		this->limitHighBit = 31;
		delete[] this->encoded;
		this->encoded = new char[16];
		for(int i = 0; i < 16; ++i) this->encoded[i] = 0;
		this->encode_length = 16;
		this->encode_pos = 0;
		this->finished = false;

		Predicter pred;
		for(int i = 0; i < length; ++i) {
			char byte = data[i];
			//this->printDebug = (i >= length - 2);
			for(int j = 0; j < 8; ++j) {
				int bit = byte & 1;
				uint64_t prob = pred.Probability();
				this->EncodeBit(bit, prob);
				pred.Update(bit, prob);
				byte >>= 1;
				}
			}

		int endPosition = this->encode_pos + this->limitHighBit;
		while(this->encode_pos < endPosition) {
			this->EncodeBit(0, 0x80000000U);
			}
		}

	const char *GetEncoded() {
		this->FinishEncoding();
		return this->encoded;
		}

	int GetEncodedLength() {
		this->FinishEncoding();
		return this->encode_pos / 8;
		}
	};

#endif