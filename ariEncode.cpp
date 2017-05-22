
#include "ariEncode.hpp"


ARIEncode::ARIEncode(BitOutputStream* bout, const unsigned symbols):counter(0), low(0), high(IntervalTraitsType::MAX), bitStreamWriter(bout) {
	cumulativeFreqs.resize(symbols);

	size_t counter = 1;
	std::generate(cumulativeFreqs.begin(), cumulativeFreqs.end(), [&counter]() { return counter++; });
	//std::cout << "max: " << IntervalTraitsType::MAX << " bits:" << IntervalTraitsType::BITS << endl;
	//cout << "cumulativ size: "<<cumulativeFreqs.size() << endl;
	//printCumulativ();
}

void ARIEncode::encode(unsigned symbol) {
	//printCumulativ();
	//cout << ">>--symbol: " << symbol << " high: " << high << " low: " << low << endl;
	uint64_t range = high - low +1;
	//cout << "sem"<< sizeof(uint32_t) << endl;
	uint64_t scale = getCumulativeFreq(cumulativeFreqs.size()-1);
	//cout << "sem" << endl;
	high = low + (range * getCumulativeFreq(symbol))/scale - 1;

	if(symbol != 0) {
		low += (range * getCumulativeFreq(symbol-1))/scale;
	}
	//printCumulativ();
	//cout << ">>--symbol: " << symbol << " high: " << high << " low: " << low << endl;

	//cout << "sem" << endl;
	//cout << "Size: " << bitStreamWriter.size() << endl;
	while(true) {
		//cout << "symbol: " <<symbol<< " high: " << high << " low: " << low << endl;
		if(high < IntervalTraitsType::HALF) {
			encodeIntervalChange(false);
		}
		else if(low >= IntervalTraitsType::HALF) {
			low -= IntervalTraitsType::HALF;
			high -= IntervalTraitsType::HALF;

			encodeIntervalChange(true);
		}
		else if(low >= IntervalTraitsType::QUARTER && high < IntervalTraitsType::THREE_QUARTERS){
			low -= IntervalTraitsType::QUARTER;
			high -= IntervalTraitsType::QUARTER;

			counter++;
		}
		else {
			break;
		}
		
		low <<=1;
		high = (high << 1) + 1;
	}
	incSymbolFreq(symbol);
}

unsigned ARIEncode::getCumulativeFreq(unsigned symbol) {
	//assert(symbol < cumulativeFreqs.size());
	//cout << "get: " << symbol << endl;
	if(symbol >= cumulativeFreqs.size()) {
		cerr << "Assert symbol < cumulativeFreqs.size()" << endl;
		exit(1);
	}
	return cumulativeFreqs[symbol];
}

void ARIEncode::computeCumulativeFreqs(vector<unsigned>& freqs) {
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