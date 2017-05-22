#ifndef ARIENCODE_H
#define ARIENCODE_H

//#include "bitStream.hpp"
#include "bitIOStream.hpp"
#include "tool.hpp"
#include <deque>

#include "ari.hpp"




//static const uint32_t MAX_FREQ = (1U << 29) - 1U;

class ARIEncode {
public:
	ARIEncode::ARIEncode(BitOutputStream* bout, const unsigned symbols);
	void ARIEncode::encode(unsigned symbol);
	virtual unsigned getCumulativeFreq(unsigned symbol);
	int writeCount;
	void close() {
		counter++;
		if(low < IntervalTraitsType::QUARTER) {
			encodeIntervalChange(false);
			}
		else {
			encodeIntervalChange(true);
			}

		bitStreamWriter->flush();
	}

private:
	
	uint32_t low;
	int counter;
	uint32_t high;
	//deque<bool> bitStreamWriter;
	BitOutputStream* bitStreamWriter;
	vector<unsigned> cumulativeFreqs;
	typedef IntervalTraits<4> IntervalTraitsType;
	void computeCumulativeFreqs(vector<unsigned>& freqs);
    
	void printCumulativ() {
		for(int i = 0; i < cumulativeFreqs.size(); i++) {
			cout << " [" << i << "] value: " << cumulativeFreqs[i];
			
		}
		cout << endl;
	}

	

	void encodeIntervalChange(bool flag) {
		//cout << "save: " << (int)flag << endl;
		bitStreamWriter->put_bit(flag);
		// handle third case, we use relation that (C3)^k C1 = C1 (C2)^k
		for(; counter > 0; --counter)
			bitStreamWriter->put_bit(!flag);
	}

	void incSymbolFreq(unsigned symbol, unsigned freq = 1) {
		bool overflow = false;
		for(auto i = symbol; i < cumulativeFreqs.size(); ++i) {
			cumulativeFreqs[i] += freq;
			// on overflow set overflow flag
			if(cumulativeFreqs[i] > MAX_FREQ)
				overflow = true;
			}

		// handle overflow we could handle it after additions because addition
		// overflows on 1bit and we have 3bits free
		if(overflow) {
			// compute frequencies
			std::vector<unsigned> freqs(cumulativeFreqs.size());
			for(size_t i = 0; i < freqs.size(); ++i)
				freqs[i] = cumulativeFreqs[i] - (i > 0 ? cumulativeFreqs[i - 1] : 0);

			// compute new cumulative freqs, overflow will be handled here
			computeCumulativeFreqs(freqs);
		}
	}

};
#endif
