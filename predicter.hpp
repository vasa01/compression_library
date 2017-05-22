#ifndef PREDICTER_H
#define PREDICTER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

class Predicter {
public:
	Predicter()
		: previousBits(0)
		{
		for(int i = 0; i < 512; ++i) this->counts[i] = 0;
		}

	uint32_t Probability() {
		int zeroes = counts[previousBits * 2];
		int ones = counts[previousBits * 2 + 1];

		uint64_t one_num = (uint64_t)(ones + 1) << 32;
		uint64_t prob = one_num / (ones + zeroes + 2);

		assert(prob > 0 && prob < 0xffffffffULL);
		return prob;
		}

	void Update(int bit, uint32_t probability) {
		assert(bit == 0 || bit == 1);
		int amount = 1;
		if(bit) this->counts[this->previousBits * 2 + 1] += amount;
		else this->counts[this->previousBits * 2] += amount;
		this->previousBits = ((this->previousBits << 1) | bit) & 0xff;
		}

	void PrintStatus() {
		printf("Predicter matrix:\n");
		for(int i = 0; i < 256; ++i) {
			printf("%4d  %4d\n", counts[i * 2], counts[i * 2 + 1]);
			}
		}

private:
	int previousBits;
	int counts[256 * 2];
	};

#endif
