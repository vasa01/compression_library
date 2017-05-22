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
 * @file	block_encoder.hpp
 *
 * @brief	Implements EBCOT block encoder.
 */
#ifndef BLOCK_CODING_H
#define	BLOCK_CODING_H

#include "bivector.hpp"
#include "block_coder.hpp"
#include "mq_encoder2.hpp"
/**
 * Block_Encoder class
 * @tparam T
 */
template<typename T>
class Block_Encoder: public block_coder {
public:

	Block_Encoder(const MyVector2i& _block_size);

	uint8_t encode(Matrix block, const subband_t& band, int plane, int min);

	vector<uint8_t> const get_buffer();
	const int32_t& get_buffer_size();

protected:

	MyVector2i block_size;
	bivector<uint16_t> states;

	std::vector<uint8_t> outbuf;
	int32_t outbuf_len;
	MQ_Encoder mq_coder;


	void cleanUpPass(Matrix block, const T& mask, const subband_t& band);
	void sigifiancePropagationPass(Matrix block, const T& mask, const subband_t& band);
	void magnitudeRefinementPass(Matrix block, const T& mask);

	inline void sign_coding(Matrix block, const int& x, const int& y);
	};
/**
 * Block_Encoder create
 * @tparam T
 * @param _block_size
 */
template<typename T>
Block_Encoder<T>::Block_Encoder(const MyVector2i& _block_size)
	: block_size(_block_size), states(block_size.x() + 2, block_size.y() + 2),
	outbuf(block_size.x() * block_size.y() * sizeof(T) * 2), mq_coder(outbuf.data(), outbuf.size()) { }

/**
 * get_buffer
 * @tparam T
 * @return buffer
 */
template<typename T>
vector<uint8_t> const Block_Encoder<T>::get_buffer() {
	return outbuf;
	}
/**
 * get_buffer_size
 * @tparam T
 * @return
 */
template<typename T>
const int32_t& Block_Encoder<T>::get_buffer_size() {
	return outbuf_len;
	}
/**
 *	encode
 * @tparam T
 * @param block
 * @param subband
 * @param plane
 * @param min
 * @return
 */
template<typename T>
uint8_t Block_Encoder<T>::encode(Matrix block, const subband_t& subband, int plane, int min) {

	states.assign(0);
	mq_coder.reset();

	const uint8_t first_sig_plane = plane;

	if(first_sig_plane >= 1) {
		cleanUpPass(block, T(1) << first_sig_plane, subband);
		for(int8_t p = first_sig_plane - 1; p >= min; --p) {
			//std::cout << "p: " << (int)p <<Tool::DecToBin(T(1) << p)<< std::endl;
			const T mask = T(1) << p;
			sigifiancePropagationPass(block, mask, subband);
			magnitudeRefinementPass(block, mask);
			cleanUpPass(block, mask, subband);
			}
		}
	mq_coder.flush();
	outbuf_len = mq_coder.get_data_len();
	return first_sig_plane + 1;
	}
/**
 * cleanUpPass
 * @tparam T
 * @param block
 * @param mask
 * @param band
 */
template<typename T>
void Block_Encoder<T>::cleanUpPass(Matrix block, const T& mask, const subband_t& band) {
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
							int pos_one;
							for(pos_one = 0; pos_one < 4; ++pos_one) {
								if(abs(int(block[cv::Point(x, y + pos_one)])) & mask) {
									break;
									}
								}

							if(pos_one < 4) {
								mq_coder.put_bit(true, 17);
								mq_coder.put_bit(pos_one & 0x2, 18);
								mq_coder.put_bit(pos_one & 0x1, 18);
								sign_coding(block, x, y + pos_one);

								for(int i = y; i <= y + pos_one; ++i) {
									states.at(x + 1, i + 1) &= ~PASS_VISIT;
									}
								y += pos_one;
								}
							else {
								mq_coder.put_bit(false, 17);
								y += 3;
								}
							}
						else {
							//zero coding
							const bool bit = abs((int)block[cv::Point(x,y)]) & mask;
							mq_coder.put_bit(bit, lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
							if(bit) {
								sign_coding(block, x, y);
								}
							}
						}
					else {
						//zero coding
						const bool bit = abs((int)block[cv::Point(x, y)]) & mask;
						mq_coder.put_bit(bit, lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
						if(bit) {
							sign_coding(block, x, y);
							}
						}
					}
				states.at(x + 1, y + 1) &= ~PASS_VISIT;
				}
			}
		}
	}
/**
 * sigifiancePropagationPass
 * @tparam T
 * @param block
 * @param mask
 * @param band
 */
template<typename T>
void Block_Encoder<T>::sigifiancePropagationPass(Matrix block, const T& mask, const subband_t& band) {
	for(int section = 0; section < block_size.y(); section += 4) {
		const int max_height = min(section + 4, block_size.y());
		for(int x = 0; x < block_size.x(); ++x) {
			for(int y = section; y < max_height; ++y) {

				uint16_t& state = states.at(x + 1, y + 1);

				if((!(state & PASS_SIG)) && (state & SIG_EIGHT)) {
					//zero coding
					const bool bit = abs(int(block[cv::Point(x, y)])) & mask;
					mq_coder.put_bit(bit, lut_zero_context[band][states.at(x + 1, y + 1) & SIG_EIGHT]);
					state |= PASS_VISIT;
					if(bit) {
						sign_coding(block, x, y);
						}
					}
				}
			}
		}
	}
/**
 * magnitudeRefinementPass
 * @tparam T
 * @param block
 * @param mask
 */
template<typename T>
void Block_Encoder<T>::magnitudeRefinementPass(Matrix block, const T& mask) {
	for(int section = 0; section < block_size.y(); section += 4) {
		const int max_height = min(section + 4, block_size.y());
		for(int x = 0; x < block_size.x(); ++x) {
			for(int y = section; y < max_height; ++y) {

				uint16_t& state = states.at(x + 1, y + 1);

				if((state & PASS_SIG) && !(state & PASS_VISIT)) {
					//magnitude refinement coding
					mq_coder.put_bit(abs(int(block[cv::Point(x, y)])) & mask, (state & PASS_REF) ? 16 : (state & SIG_EIGHT) ? 15 : 14);
					state |= PASS_REF;
					}
				}
			}
		}
	}
/**
 * sign_coding
 * @tparam T
 * @param block
 * @param x
 * @param y
 */
template<typename T>
void Block_Encoder<T>::sign_coding(Matrix block, const int& x, const int& y) {

	uint16_t& state = states.at(x + 1, y + 1);

	// sign coding
	const uint8_t sign_state = (state & (SIG_FOUR | SGN_FOUR)) >> 4;
	mq_coder.put_bit(
		(block[cv::Point(x, y)] < 0) ^ lut_sign_prediction[sign_state],
		lut_sign_context[sign_state]
	);

	if(block[cv::Point(x, y)] < 0) {
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
#endif	/* BLOCK_CODING_H */