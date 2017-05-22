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
 * @file	mq_decoder2.hpp
 *
 * @brief	mq decoder implementation.
 */

#ifndef MQ_DECODER_H
#define	MQ_DECODER_H

#include <vector>
#include "mq_coder.hpp"

/**
 * class mq_decoder
 */
class mq_decoder: public mq_coder {
public:


	mq_decoder();

	void reset(const uint8_t *_buf, const int32_t& max_len);


	bool get_bit(const uint8_t& ctx);

protected:

	const uint8_t *inbuf;
	int32_t inbuf_pos, inbuf_max_len;

private:

	uint32_t C, A, CT;

	void init();
	inline bool exchange_mps(const uint8_t& ctx);
	inline bool exchange_lps(const uint8_t& ctx);
	inline void byte_in();
	inline void renorm();
	};

#endif