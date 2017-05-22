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
 * @file	ariEecode.hpp
 *
 * @brief	arithmetic encoder.
 */

#ifndef ARIENCODE_H
#define ARIENCODE_H

//#include "bitStream.hpp"
#include "bitIOStream.hpp"
#include "tool.hpp"
#include <deque>

#include "ari.hpp"




//static const uint32_t MAX_FREQ = (1U << 29) - 1U;
/**
 * class ARIEncode
 */
class ARIEncode {
public:
	ARIEncode(BitOutputStream* bout, const unsigned symbols);
	void encode(unsigned symbol);
	virtual unsigned getCumulativeFreq(unsigned symbol);
	int writeCount;
	/**
	 * close
	 */
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
	 * encodeIntervalChange
	 * @param flag
	 */
	void encodeIntervalChange(bool flag) {
		//cout << "save: " << (int)flag << endl;
		bitStreamWriter->put_bit(flag);
		// handle third case, we use relation that (C3)^k C1 = C1 (C2)^k
		for(; counter > 0; --counter)
			bitStreamWriter->put_bit(!flag);
	}

	/**
	 * incSymbolFreq - increment symbol freq
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
