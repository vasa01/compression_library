

#include "EZWEncode.hpp"

EZWEncode::EZWEncode() {
	
	bitStreamWriter = new BitOutputStream(&temp);
}
BitOutputStream* EZWEncode::getBitstream(){
	return bitStreamWriter;
}

ostringstream* EZWEncode::getBitstreamString() {
	return &temp;
}

deque<uint8_t> EZWEncode::getSymbols() {
	return symbols;
}

int32_t EZWEncode::getThreshold() {
	return defaultThreshold;
}

int32_t EZWEncode::computeThreshold(Matrix mat) {
    Matrix abs = Tool::Abs(mat);
	int32_t max = abs.getMax();

    return 1 << static_cast<int32_t>(floor(log10(max-1) / log10(2.0)));
}

void EZWEncode::encode( Matrix& mat, int32_t minThreshold) {
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

Element EZWEncode::codeElm(Matrix& mat, int x, int y, int32_t threshold) {
    Element result(x, y);
    result.code = computeElmCode(mat, x, y, threshold);
    if(result.code == Element::Code::Pos || result.code == Element::Code::Neg) {
        subordList.push_back(abs(mat.get(y, x)));
        mat.set(y,x,0);
    }

    return result;
   
}

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

void EZWEncode::outputCode(Element::Code code) {
	symbols.push_back(static_cast<unsigned>(code));
}