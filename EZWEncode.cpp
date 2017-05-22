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
 * @file	EZWEncode.cpp
 *
 * @brief	EZWEncode implementation.
*/
#include "EZWEncode.hpp"
/**
 * create EZWEncode
 */
EZWEncode::EZWEncode() {
	
	bitStreamWriter = new BitOutputStream(&temp);
}
/**
 * getBitstream
 * @return - BitOutputStream
 */
BitOutputStream* EZWEncode::getBitstream(){
	return bitStreamWriter;
}

/**
 * getBitstreamString
 * @return
 */
ostringstream* EZWEncode::getBitstreamString() {
	return &temp;
}

/**
 * getSymbols
 * @return - vector symbols
 */
deque<uint8_t> EZWEncode::getSymbols() {
	return symbols;
}

/**
 * getThreshold
 * @return - threshold
 */
int32_t EZWEncode::getThreshold() {
	return defaultThreshold;
}

/**
 * computeThreshold in Matrix
 * @param mat - Matrix
 * @return - threshold
 */
int32_t EZWEncode::computeThreshold(Matrix mat) {
    Matrix abs = Tool::Abs(mat);
	int32_t max = abs.getMax();

    return 1 << static_cast<int32_t>(floor(log10(max-1) / log10(2.0)));
}

/**
 * encode
 * @param mat - input Matrix
 * @param minThreshold
 */
void EZWEncode::encode( Matrix mat, int32_t minThreshold) {
	Matrix abs = Tool::Abs(mat);
	int32_t max = abs.getMax();
    uint32_t threshold = computeThreshold(mat);
	minThreshold = threshold >> minThreshold;
	cout << "threshold:"<< threshold << endl;
	defaultThreshold = threshold;

    do {
        //dataModel.reset();
        dominantPass(mat, threshold);
		cout << threshold << endl;
        subordinatePass(threshold, minThreshold);

        threshold >>= 1;		// shift to right by one means divide by two
    } while(threshold > minThreshold);
 
}
/**
 * init
 * @param mat
 * @param threshold
 */
void EZWEncode::init(Matrix& mat, int32_t threshold) {
    Element elm;

    elm = codeElm(mat,0,0,threshold);
    outputCode(elm.code);

    elm = codeElm(mat, 1, 0, threshold);
    elements.push_back(elm);

    elm = codeElm(mat, 0, 1, threshold);
    elements.push_back(elm);

    elm = codeElm(mat, 1, 1, threshold);
    elements.push_back(elm);
}

/**
 * codeElm
 * @param mat
 * @param x
 * @param y
 * @param threshold
 * @return
 */
Element EZWEncode::codeElm(Matrix& mat, int x, int y, int32_t threshold) {
    Element result(x, y);
    result.code = computeElmCode(mat, x, y, threshold);
    if(result.code == Element::Code::Pos || result.code == Element::Code::Neg) {
        subordList.push_back(abs(mat.get(y, x)));
        mat.set(y,x,0);
    }

    return result;
   
}

/**
 * computeElmCode
 * @param mat
 * @param x
 * @param y
 * @param threshold
 * @return - Element::Code
 */
Element::Code EZWEncode::computeElmCode(Matrix& mat, int x, int y, int32_t threshold) {
	int32_t coef = mat.get(y, x);

    if(abs(coef) >= threshold) {
        if(coef >= 0)
            return Element::Code::Pos;
        else
            return Element::Code::Neg;
        }
    else {
        if(isZerotreeRoot(mat, x, y, threshold))
            return Element::Code::ZeroTreeRoot;
        else
            return Element::Code::IsolatedZero;
        }

}

/**
 * isZerotreeRoot
 * @param mat
 * @param x
 * @param y
 * @param threshold
 * @return  - bit
 */
bool EZWEncode::isZerotreeRoot(Matrix& mat, int x, int y, int32_t threshold) {
        if(x == 0 && y == 0) {
            int32_t tmp = mat.get(y, x);
            mat.set(y,x, std::numeric_limits<int32_t>::min());
            int32_t absmax = Tool::Abs(mat).getMax();
            mat.set(y,x,tmp);
            if(absmax >= threshold)
                return false;
            return true;
        }

        auto minx = x * 2;
        auto miny = y * 2;
        auto maxx = (x + 1) * 2;
        auto maxy = (y + 1) * 2;
        if(minx == mat.w() || miny == mat.h())
            return true;

        while(maxx <= (size_t)mat.w() && maxy <= (size_t)mat.h()) {
            for(auto i = miny; i < maxy; ++i) {
                for(auto j = minx; j < maxx; ++j) {
                    if(abs(mat.get(i, j)) >= threshold)
                        return false;
                }
            }

            minx *= 2;
            miny *= 2;
            maxx *= 2;
            maxy *= 2;
        }

        return true;
}

/**
 * dominantPass
 * @param mat
 * @param threshold
 */
void EZWEncode::dominantPass(Matrix& mat, int32_t threshold) {
	init(mat,threshold);

	do {
		auto elm = elements.front();
		elements.pop_front();
		outputCode(elm.code);

		if(elm.code != Element::Code::ZeroTreeRoot) {
			//cout << "="<< elements.size() << endl;
			auto minx = elm.x * 2;
			auto miny = elm.y * 2;
			auto maxx = minx + 1;
			auto maxy = miny + 1;
			if(maxx <= (size_t)mat.w() && maxy <= (size_t)mat.h()) {	
				for(auto y = miny; y < maxy + 1; ++y) {
					for(auto x = minx; x < maxx + 1; ++x) {
						auto child = codeElm(mat, x, y, threshold);
						elements.push_back(child);
						}
					}
				}
			}

		} while(!elements.empty());
}

/**
 * subordinatePass
 * @param threshold
 * @param minThreshold
 */
void EZWEncode::subordinatePass(int32_t threshold, int32_t minThreshold) {
	threshold >>= 1;					
	if(threshold <= minThreshold)
		return;

	for(auto elm : subordList) {
		if((elm & threshold) != 0) {
			bitStreamWriter->put_bit(true);

		}
		else {
			bitStreamWriter->put_bit(false);
		}
	}
}

/**
 * outputCode
 * @param code
 */
void EZWEncode::outputCode(Element::Code code) {
	symbols.push_back(static_cast<unsigned>(code));
}
