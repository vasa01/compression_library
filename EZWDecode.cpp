
#include "EZWDecode.hpp"

EZWDecode::EZWDecode(deque<unsigned> symbols, BitInputStream* bitStreamReader):pixels(0), symbols(symbols), bitStreamReader(bitStreamReader) {

}

void EZWDecode::decode(int32_t threshold, int32_t minThreshold, Matrix& mat) {
	//cout << "symbolsize: " << symbols.size() << "bitstreamsize: " << endl;
	minThreshold = threshold >> minThreshold;
	do {
		//dataModel.reset();

		dominantPass(threshold, mat);
		//cout << "after dom" << endl;
		subordinatePass(threshold, minThreshold, mat);
		//cout << "after sub" << endl;
		//cout << "threshold:" << threshold << endl;
		threshold >>= 1;
	} while(threshold > minThreshold);

/*#ifdef DUMP_RES
		std::cerr << std::endl;
#endif*/
}

void EZWDecode::dominantPass(int32_t threshold, Matrix& mat) {
	init(threshold, mat);
	//cout << "begin dom" << endl;
	do {
		//cout << "do" << endl;
		auto elm = elements.front();
		elements.pop_front();
		//Tool::PopFront(elements);
		if(elm.code == Element::Code::Neg || elm.code == Element::Code::Pos)
			pixels++;
		//cout << "pixels" << endl;
		if(elm.code != Element::Code::ZeroTreeRoot) {
			// handle elm children
			auto minx = elm.x * 2;
			auto miny = elm.y * 2;
			auto maxx = minx + 1;
			auto maxy = miny + 1;
			//cout << "inzero" << endl;
			if(maxx <= (size_t)mat.w() && maxy <= (size_t)mat.h()) {						// last level doesn't have children
				for(auto y = miny; y < maxy + 1; ++y) {
					for(auto x = minx; x < maxx + 1; ++x) {
						auto child = decodeElement(threshold, x, y, mat);
						elements.push_back(child);
					}
				}
			}
			//cout << "after inzero" << endl;
		}
			

	} while(!elements.empty());
}

void EZWDecode::subordinatePass(int32_t threshold, int32_t minThreshold, Matrix& mat) {
	threshold >>= 1;
	if(threshold <= minThreshold)
		return;

	for(size_t i = 0; i < pixels; ++i) {
		auto coord = subordVec[i];
		auto elm = mat.get(coord.y, coord.x);
		bool readbit = bitStreamReader->get_bit();// .front();
		//Tool::PopFront(bitStreamReader);
		//bitStreamReader.pop_front();
		if(readbit) {
/*#ifdef DUMP_RES
			std::cerr << "1";
#endif*/
			if(elm < 0)
				mat.set(coord.y, coord.x, elm - threshold);// at<int32_t>(coord.y, coord.x) = elm - threshold;
			else
				mat.set(coord.y, coord.x, elm + threshold);//mat.at<int32_t>(coord.y, coord.x) = elm + threshold;
			}
/*#ifdef DUMP_RES
		else
			std::cerr << "0";
#endif*/
		}
}

void EZWDecode::init(int32_t threshold, Matrix& m) {
	Element elm;

	elm = decodeElement(threshold, 0, 0, m);
	if(elm.code == Element::Code::Neg || elm.code == Element::Code::Pos)
		pixels++;

	elm = decodeElement(threshold, 1, 0, m);
	elements.push_back(elm);

	elm = decodeElement(threshold, 0, 1, m);
	elements.push_back(elm);

	elm = decodeElement(threshold, 1, 1, m);
	elements.push_back(elm);
}

Element EZWDecode::decodeElement(int32_t threshold, int x, int y, Matrix& m) {
	Element result(x, y);
	result.code = readElementCode();

	if(result.code == Element::Code::Pos) {
		m.set(y, x, threshold);//  m.at<int32_t>(y, x) = threshold;
		//cout << m[cv::Point(x,y)] << endl;
		subordVec.push_back(ElementCoord(x, y));
	}
	else if(result.code == Element::Code::Neg) {
		m.set(y, x, -threshold);//m.at<int32_t>(y, x) = -threshold;
		subordVec.push_back(ElementCoord(x, y));
	}

	return result;
}

Element::Code EZWDecode::readElementCode() {
	if (symbols.empty()) {
		cerr << "Error: symbols empty!!!<><>" << endl;
		exit(1);
	}
	auto code = static_cast<Element::Code>(symbols.front());//static_cast<Element::Code>(adecoder->decode(&dataModel));
	//Tool::PopFront(symbols);
	symbols.pop_front();
/*#ifdef DUMP_RES
	switch(code)
		{
		case EzwCodec::Element::Code::Pos:
			std::cerr << "p";
			break;
		case EzwCodec::Element::Code::Neg:
			std::cerr << "n";
			break;
		case EzwCodec::Element::Code::IsolatedZero:
			std::cerr << "z";
			break;
		case EzwCodec::Element::Code::ZeroTreeRoot:
			std::cerr << "t";
			break;
		default:
			break;
		}
#endif*/
	return code;

}