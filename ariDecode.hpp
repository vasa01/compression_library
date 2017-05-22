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
 * @file	ariDecode.hpp
 *
 * @brief	arithmetic decoder.
 */

#ifndef ARIDECODE_H
#define ARIDECODE_H

#include "bitIOStream.hpp"
#include "tool.hpp"
#include <deque>
#include "ari.hpp"
//#include <memory>



/**
 * Class ARIDecode
 */
class ARIDecode {
public:
	ARIDecode(BitInputStream* bst, const unsigned symbols);
	unsigned decode();

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

	/**
	 * printCumulativ
	 */
	void printCumulativ() {
		for(int i = 0; i < cumulativeFreqs.size(); i++) {
			cout << " [" << i << "] value: " << cumulativeFreqs[i];

			}
		cout << endl;
	}

	/**
	 * getCumulativeFreq
	 * @param symbol
	 * @return
	 */
	unsigned getCumulativeFreq(unsigned symbol) const {
		//assert(symbol < cumulativeFreqs.size());
		if(symbol >= cumulativeFreqs.size()) {
			cerr << "Assert symbol < cumulativeFreqs.size()" << endl;
			exit(1);
			}
		return cumulativeFreqs[symbol];
	}

	/**
	 * incSymbolFreq
	 * @param symbol
	 * @param freq
	 */
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
