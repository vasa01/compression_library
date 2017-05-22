#include "mq_decoder2.hpp"

mq_decoder::mq_decoder()
	{
	}

void mq_decoder::reset(const uint8_t *_buf, const int32_t& max_len)
	{
	inbuf = _buf;
	inbuf_max_len = max_len;
	inbuf_pos = 0;
	init();
	}

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

void mq_decoder::init()// throw (error)
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

void mq_decoder::byte_in() //throw (error)
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

void mq_decoder::renorm()// throw (error)
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
