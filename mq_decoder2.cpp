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
 * @file	mq_decoder2.cpp
 *
 * @brief	mq decoder implementation.
 */

#include "mq_decoder2.hpp"

mq_decoder::mq_decoder()
	{
	}

/**
 * reset mq decoder
 * @param _buf
 * @param max_len
 */
void mq_decoder::reset(const uint8_t *_buf, const int32_t& max_len)
	{
	inbuf = _buf;
	inbuf_max_len = max_len;
	inbuf_pos = 0;
	init();
	}

/**
 * get bit by ctx
 * @param ctx
 * @return
 */
bool mq_decoder::get_bit(const uint8_t& ctx) //throw (error)
	{
	bool bit;

	const state_t& state = *state_mapping[ctx];
	const uint16_t& qeval = state.qeval;

	A -= qeval;
	if((C >> 16) < qeval) {
		bit = exchange_lps(ctx);
		renorm();
		}
	else {
		C -= (qeval << 16);
		if(A & 0x8000) {
			bit = state.mps;
			}
		else {
			bit = exchange_mps(ctx);
			renorm();
			}
		}

	return bit;
	}

/**
 * init mq decoder
 */
void mq_decoder::init()
	{
	mq_coder::init();

	if(inbuf_max_len == 0) {
		C = (0xFF << 16);
		}
	else {
		C = (inbuf[inbuf_pos] << 16);
		}

	CT = 0;
	byte_in();
	C <<= 7;
	CT -= 7;
	A = 0x8000;
	}

/**
 * exchange_mps
 * @param ctx
 * @return - bit
 */
bool mq_decoder::exchange_mps(const uint8_t& ctx)
	{
	const state_t& state = *state_mapping[ctx];

	if(A < state.qeval) {
		state_mapping[ctx] = state.nlps;
		return 1 - state.mps;
		}
	else {
		state_mapping[ctx] = state.nmps;
		return state.mps;
		}
	}

/**
 * exchange_lps
 * @param ctx
 * @return  - bit
 */
bool mq_decoder::exchange_lps(const uint8_t& ctx)
	{
	const state_t& state = *state_mapping[ctx];

	if(A < state.qeval) {
		A = state.qeval;
		state_mapping[ctx] = state.nmps;
		return state.mps;
		}
	else {
		A = state.qeval;
		state_mapping[ctx] = state.nlps;
		return 1 - state.mps;
		}
	}

/**
 * byte_in
 */
void mq_decoder::byte_in()
	{
	if(inbuf_pos < inbuf_max_len) {
		uint8_t buf;
		if(inbuf_pos + 1 < inbuf_max_len) {
			buf = inbuf[inbuf_pos + 1];
			}
		else {
			buf = 0xFF;
			}

		if(inbuf[inbuf_pos] == 0xFF) {
			if(buf > 0x8F) {
				C += 0xFF00;
				CT = 8;
				}
			else {
				C += (buf << 9);
				CT = 7;
				inbuf_pos++;
				}
			}
		else {
			C += (buf << 8);
			CT = 8;
			inbuf_pos++;
			}
		}
	else {
		C += 0xFF00;
		CT = 8;
		}
	}
/**
 * renorm
 */
void mq_decoder::renorm()
	{
	do {
		if(CT == 0) {
			byte_in();
			}
		A <<= 1;
		C <<= 1;
		CT--;
		} while(A < 0x8000);
	}
