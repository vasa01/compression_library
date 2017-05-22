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
 * @file	mq_encoder2.cpp
 *
 * @brief	mq encoder implementation.
 */

#include "mq_encoder2.hpp"

MQ_Encoder::MQ_Encoder()
	{
	init();
	}

/**
 * MQ_Encoder
 * @param _buf
 * @param max_len
 */
MQ_Encoder::MQ_Encoder(uint8_t *_buf, const int32_t& max_len): outbuf(_buf), outbuf_pos(-1), outbuf_max_len(max_len)
	{
	init();
	}
/**
 * reset
 */
void MQ_Encoder::reset()
	{
	outbuf_pos = -1;
	init();
	}

/**
 * get data len
 * @return - len buf
 */
int32_t MQ_Encoder::get_data_len()
	{
	return outbuf_pos + 1;
	}

/**
 * Put bit and ctx
 * @param bit
 * @param ctx
 */
void MQ_Encoder::put_bit(const bool& bit, const uint8_t& ctx)// throw (error)
	{
	/*if(outbuf_pos < 2){
		std::cout << "ctx: " << (int)ctx << std::endl;
	}*/
	if(bit == state_mapping[ctx]->mps) {
		code_mps(ctx);
		}
	else {
		code_lps(ctx);
		}
	}

/**
 * flush
 */
void MQ_Encoder::flush()
	{
	const uint32_t tmp_C = C + A;
	C |= 0xFFFF;
	if(C >= tmp_C) {
		C -= 0x8000;
		}
	C <<= CT;
	byte_out();
	C <<= CT;
	byte_out();

	if(outbuf[outbuf_pos] == 0xFF) {
		outbuf_pos--;
		}
	}

/**
 * init
 */
void MQ_Encoder::init()
	{
	mq_coder::init();

	A = 0x8000;
	C = 0;
	CT = 12;
	outbuf_pos = -1;
	}

/**
 * code MPS ctx
 * @param cx
 */
void MQ_Encoder::code_mps(const uint8_t& cx) //throw (error)
	{
	const state_t& state = *state_mapping[cx];
	const uint32_t& qeval = state.qeval;

	A -= qeval;

	if(!(A & 0x8000)) {
		if(A < qeval) {
			A = qeval;
			}
		else {
			C += qeval;
			}
		state_mapping[cx] = state.nmps;
		renorm();
		}
	else {
		C += qeval;
		}
	}

/**
 * code_lps
 * @param cx
 */
void MQ_Encoder::code_lps(const uint8_t & cx)// throw (error)
	{
	const state_t& state = *state_mapping[cx];
	const uint32_t& qeval = state.qeval;

	A -= qeval;

	if(A < qeval) {
		C += qeval;
		}
	else {
		A = qeval;
		}

	state_mapping[cx] = state.nlps;
	renorm();
	}

/**
 * renorm on byte
 */
void MQ_Encoder::renorm()// throw (error)
	{
		do {
			A <<= 1;
			C <<= 1;
			CT--;
			if(CT == 0) {
				byte_out();
			}
		} while(!(A & 0x8000));
	}

/**
 * byte out to buffer
 */
void MQ_Encoder::byte_out()// throw (error)
	{
	if(outbuf_pos < 0 || outbuf[outbuf_pos] != 0xFF) {
		if((C & 0x8000000) == 0) {
			if(outbuf_pos + 1 >= outbuf_max_len) {std::cerr << "premature end of buffer reached"<< std::endl; }
			outbuf[++outbuf_pos] = (C >> 19);
			C &= 0x7FFFF;
			CT = 8;
			}
		else {
			if(++outbuf[outbuf_pos] == 0xFF) {
				C &= 0x7FFFFFF;
				if(outbuf_pos + 1 >= outbuf_max_len) { std::cerr << "premature end of buffer reached" << std::endl; }
				outbuf[++outbuf_pos] = (C >> 20);
				C &= 0xFFFFF;
				CT = 7;
				}
			else {
				if(outbuf_pos + 1 >= outbuf_max_len) { std::cerr << "premature end of buffer reached" << std::endl; }
				outbuf[++outbuf_pos] = (C >> 19);
				C &= 0x7FFFF;
				CT = 8;
				}
			}
		}
	else {
		if(outbuf_pos >= outbuf_max_len) { std::cerr << "premature end of buffer reached" << std::endl; }
		outbuf[++outbuf_pos] = (C >> 20);
		C &= 0xFFFFF;
		CT = 7;
		}
	}