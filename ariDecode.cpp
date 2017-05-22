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
 * @file	ariDecode.cpp
 *
 * @brief	arithmetic decoder.
 */

#include "ariDecode.hpp"

/**
 * ARIDecode
 * @param bsr
 * @param symbols
 */
ARIDecode::ARIDecode(BitInputStream* bsr, const unsigned symbols):low(0),high(IntervalTraitsType::MAX), bitStreamReader(bsr) {
	cumulativeFreqs.resize(symbols);

	size_t counter = 1;
	std::generate(cumulativeFreqs.begin(), cumulativeFreqs.end(), [&counter]() { return counter++; });
	//cout << IntervalTraitsType::BITS << endl;
	value = 0;
	for(int i = 0; i < IntervalTraitsType::BITS; ++i) {
		//cout << "redbit 1" << endl;
		readBit();
		//cout << "redbit 2" << endl;
	}
}
/**
 * readBit
 */
void ARIDecode::readBit() {
	bool bit;
	//cout << "size: " << bitStreamReader. << endl;
	try {

		bit = bitStreamReader->get_bit();
		//cout << bit << endl;
	}
	catch(std::exception&) {
		bit = false;		// on data end we append zero bit
		//cout << "doslo" << endl;
	}

	value <<= 1;
	if(bit)
		value += 1;
}

/**
 * @fn  decode
 * @return
 */
unsigned ARIDecode::decode() {
	//printCumulativ();
	//cout << ">>--high: " << high << " low: " << low << endl;
	uint64_t range = high - low + 1;
	//cout << "decode " << endl;
	auto scale = getCumulativeFreq(cumulativeFreqs.size() - 1);
	//cout << "decode " << endl;
	auto cumulativeFreq = ((value - low + 1) * scale - 1) / range;

	//cout << "decode " << endl;

	unsigned symbol = 0;
	// find i where cumuliveFreqs[i-1] < cumulativeFreq < cumulativeFreqs[i]
	for(size_t i = 0; i < cumulativeFreqs.size(); ++i) {
		auto lowerBound = i != 0 ? getCumulativeFreq(i - 1) : 0U;
		auto upperBound = getCumulativeFreq(i);
		if(lowerBound <= cumulativeFreq && cumulativeFreq < upperBound) {
			symbol = i;
			break;
		}
	}
	high = low + (range * getCumulativeFreq(symbol)) / scale - 1;

	if(symbol != 0) {
		low += (range * getCumulativeFreq(symbol - 1)) / scale;
	}

	//printCumulativ();
	//cout << ">>--high: " << high << " low: " << low << endl;

	while(1) {
		if(high < IntervalTraitsType::HALF) {
			;	// do nothing
				// interval is in upper half of possible range
			}
		else if(low >= IntervalTraitsType::HALF) {
			low -= IntervalTraitsType::HALF;
			high -= IntervalTraitsType::HALF;
			value -= IntervalTraitsType::HALF;
			// interval is in middle of possible range
			}
		else if(low >= IntervalTraitsType::QUARTER && high <= IntervalTraitsType::THREE_QUARTERS) {
			low -= IntervalTraitsType::QUARTER;
			high -= IntervalTraitsType::QUARTER;
			value -= IntervalTraitsType::QUARTER;
			// none of these cases so break loop
			}
		else
			break;

		low <<= 1;
		high = (high << 1) + 1;
		//cout << "1___semhle" << endl;
		readBit();
		//cout << "2___semhle" << endl;
		
	}
	//cout << "semhle" << endl;
	incSymbolFreq(symbol);

	return symbol;
	
}


/**
 * computeCumulativeFreqs
 * @param freqs
 */
void ARIDecode::computeCumulativeFreqs(vector<unsigned>& freqs) {
	cumulativeFreqs.resize(freqs.size());

	for(size_t i = 0; i < cumulativeFreqs.size(); ++i) {
		cumulativeFreqs[i] = i > 0 ? (cumulativeFreqs[i - 1] + freqs[i]) : freqs[i];
		// handle overflow
		if(cumulativeFreqs[i] > MAX_FREQ) {
			// divide all frequencies bigger than 1 by two and compute new cumulative frequencies
			auto newFreqs = freqs;
			for(auto& f : newFreqs) {
				if(f > 1)
					f /= 2;
				}
			computeCumulativeFreqs(newFreqs);
			break;
			}
		}
}