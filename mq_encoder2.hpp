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
 * @file	mq_encoder2.hpp
 *
 * @brief	mq encoder implementation.
 */

#ifndef MQ_CODING_H
#define	MQ_CODING_H

#include <vector>

//#include "common/error.h"
#include "mq_coder.hpp"
#include <iostream>

/**
 * class MQ_Encoder
 */
class MQ_Encoder: public mq_coder {
public:

	MQ_Encoder();

	/**
	 *
	 * @param _buf - pointer output buffer
	 * @param max_len - max_len output buffer
	 */
	MQ_Encoder(uint8_t *_buf, const int32_t& max_len);

	/**
	 *
	 */
	void reset();

	/**
	 * get_buffer
	 * @return  - buffer
	 */
	std::vector<uint8_t>  get_buffer() {
		std::vector<uint8_t> buf;
		for(int i = 0; i < this->get_data_len(); i++) {
			buf.push_back(outbuf[i]);
		}
		return buf;
	}
	int32_t get_data_len();

	/** Encodes a new decision-bit with given context
	*  @throws error if an error occured while writing to the output stream
	*/
	void put_bit(const bool& bit, const uint8_t& ctx);// throw (error);

	/** Flushes all encoded bits into the output stream*/
	void flush();// throw (error);

protected:
	uint8_t *outbuf;
	int32_t outbuf_pos, outbuf_max_len;

private:

	void init();
	inline void code_mps(const uint8_t& cx);// throw (error);
	inline void code_lps(const uint8_t& cx);// throw (error);
	inline void renorm();// throw (error);

	inline void byte_out();// throw (error);

	uint32_t A, C, CT;
	};

#endif	/* MQ_CODING_H */