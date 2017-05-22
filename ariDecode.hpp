#ifndef ARIDECODE_H
#define ARIDECODE_H

#include "bitIOStream.hpp"
#include "tool.hpp"
#include <deque>
#include "ari.hpp"
//#include <memory>




class ARIDecode {
public:
	ARIDecode::ARIDecode(BitInputStream* bst, const unsigned symbols);
	unsigned ARIDecode::decode();

private:

	uint64_t low;
	int value;
	vector<unsigned> cumulativeFreqs;
	uint64_t high;
	BitInputStream* bitStreamReader;
	//vector<unsigned> cumulativeFreqs;
	void computeCumulativeFreqs(vector<unsigned>& freqs);
	void readBit();
	typedef IntervalTraits<4> IntervalTraitsType;

	void printCumulativ() {
		for(int i = 0; i < cumulativeFreqs.size(); i++) {
			cout << " [" << i << "] value: " << cumulativeFreqs[i];

			}
		cout << endl;
	}
	
	unsigned getCumulativeFreq(unsigned symbol) const {
		//assert(symbol < cumulativeFreqs.size());
		if(symbol >= cumulativeFreqs.size()) {
			cerr << "Assert symbol < cumulativeFreqs.size()" << endl;
			exit(1);
			}
		return cumulativeFreqs[symbol];
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
