#ifndef BLOCK_DECODER_H
#define	BLOCK_DECODER_H

//#include "common/bivector.h"
#include "block_coder.hpp"
#include "mq_decoder2.hpp"

template<typename T>
class block_decoder: public block_coder {
public:

	block_decoder(const MyVector2i& _block_size);

	void decode(uint8_t* buffer, const int32_t& buflen, const uint8_t& planes, bivector<T>& block, const subband_t& band, int min);

protected:

	MyVector2i block_size;
	bivector<uint16_t> states;

	mq_decoder mqcoder;

	void cleanUpPass(bivector<T>& block, const subband_t& band);
	void sigifiancePropagationPass(bivector<T>& block, const subband_t& band);
	void magnitudeRefinementPass(bivector<T>& block);

	inline void sign_coding(bivector<T>& block, const int& x, const int& y);
	};

template<typename T>
block_decoder<T>::block_decoder(const MyVector2i& _block_size)
	: block_size(_block_size), states(block_size.x() + 2, block_size.y() + 2) { }

template<typename T>
void block_decoder<T>::decode(uint8_t* buffer, const int32_t& buflen, const uint8_t& planes, bivector<T>& block, const subband_t& band, int min) {

	states.assign(0);
	mqcoder.reset(buffer, buflen);
	cleanUpPass(block, band);
	for(int p = 1; p < planes-min; ++p) {
		sigifiancePropagationPass(block, band);
		magnitudeRefinementPass(block);
		cleanUpPass(block, band);
	}
}

template<typename T>
void block_decoder<T>::cleanUpPass(bivector<T>& block, const subband_t& band) {
	for(int section = 0; section < block_size.y(); section += 4) {
		const int max_height = min(section + 4, block_size.y());
		for(int x = 0; x < block_size.x(); ++x) {
			for(int y = section; y < max_height; ++y) {

				if(!(states.at(x + 1, y + 1) & (PASS_SIG | PASS_VISIT))) {
					if(y % 4 == 0 && y + 3 < block_size.y()) {
						if(
							!(states.at(x + 1, y + 1) & SIG_EIGHT)
							&& !(states.at(x + 1, y + 2) & (PASS_SIG | SIG_EIGHT))
							&& !(states.at(x + 1, y + 3) & (PASS_SIG | SIG_EIGHT))
							&& !(states.at(x + 1, y + 4) & (PASS_SIG | SIG_EIGHT))
							) {
							const bool has_one = mqcoder.get_bit(17);

							int end = y + 3;
							if(has_one) {
								int pos_one = mqcoder.get_bit(18) << 1;
								pos_one |= (mqcoder.get_bit(18));
								end = y + pos_one;
								block.at(x, end) += 1;
								sign_coding(block, x, end);
								}

							for(int i = y; i <= end; ++i) {
								states.at(x + 1, i + 1) &= ~PASS_VISIT;
								}
							y = end;
							}
						else {
							//zero coding
							const bool bit = mqcoder.get_bit(lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
							if(bit) {
								block.at(x, y) += 1;
								sign_coding(block, x, y);
								}
							}
						}
					else {
						//zero coding
						const bool bit = mqcoder.get_bit(lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
						if(bit) {
							block.at(x, y) += 1;
							sign_coding(block, x, y);
							}
						}
					}
				states.at(x + 1, y + 1) &= ~PASS_VISIT;
				}
			}
		}
	}

template<typename T>
void block_decoder<T>::sigifiancePropagationPass(bivector<T>& block, const subband_t& band) {
	for(int section = 0; section < block_size.y(); section += 4) {
		const int max_height = min(section + 4, block_size.y());
		for(int x = 0; x < block_size.x(); ++x) {
			for(int y = section; y < max_height; ++y) {

				block.at(x, y) <<= 1;

				uint16_t& state = states.at(x + 1, y + 1);
				if((!(state & PASS_SIG)) && (state & SIG_EIGHT)) {
					//zero coding
					const bool bit = mqcoder.get_bit(lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
					state |= PASS_VISIT;
					if(bit) {
						block.at(x, y) += 1;
						sign_coding(block, x, y);
						}
					}
				}
			}
		}
	}

template<typename T>
void block_decoder<T>::magnitudeRefinementPass(bivector<T>& block) {
	for(int section = 0; section < block_size.y(); section += 4) {
		const int max_height = min(section + 4, block_size.y());
		for(int x = 0; x < block_size.x(); ++x) {
			for(int y = section; y < max_height; ++y) {

				uint16_t& state = states.at(x + 1, y + 1);
				if((state & PASS_SIG) && !(state & PASS_VISIT)) {
					//magnitude refinement coding
					if(mqcoder.get_bit((state & PASS_REF) ? 16 : (state & SIG_EIGHT) ? 15 : 14)) {
						block.at(x, y) += (block.at(x, y) < 0) ? -1 : 1;
						}
					state |= PASS_REF;
					}
				}
			}
		}
	}

template<typename T>
void block_decoder<T>::sign_coding(bivector<T>& block, const int& x, const int& y) {
	uint16_t& state = states.at(x + 1, y + 1);

	// sign coding
	const uint8_t sign_state = (states.at(x + 1, y + 1) & (SIG_FOUR | SGN_FOUR)) >> 4;
	const bool sign = mqcoder.get_bit(lut_sign_context[sign_state]) ^ lut_sign_prediction[sign_state];

	if(sign) {
		block.at(x, y) *= -1;
		states.at(x + 1, y + 0) |= SGN_S;
		states.at(x + 0, y + 1) |= SGN_E;
		states.at(x + 2, y + 1) |= SGN_W;
		states.at(x + 1, y + 2) |= SGN_N;
		}

	state |= PASS_SIG;
	states.at(x + 0, y + 0) |= SIG_SE;
	states.at(x + 1, y + 0) |= SIG_S;
	states.at(x + 2, y + 0) |= SIG_SW;
	states.at(x + 0, y + 1) |= SIG_E;
	states.at(x + 2, y + 1) |= SIG_W;
	states.at(x + 0, y + 2) |= SIG_NE;
	states.at(x + 1, y + 2) |= SIG_N;
	states.at(x + 2, y + 2) |= SIG_NW;
	}

#endif	/* BLOCK_DECODER_H */
