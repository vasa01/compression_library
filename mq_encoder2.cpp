#include "mq_encoder2.hpp"

MQ_Encoder::MQ_Encoder()
	{
	init();
	}

MQ_Encoder::MQ_Encoder(uint8_t *_buf, const int32_t& max_len): outbuf(_buf), outbuf_pos(-1), outbuf_max_len(max_len)
	{
	init();
	}

void MQ_Encoder::reset()
	{
	outbuf_pos = -1;
	init();
	}

int32_t MQ_Encoder::get_data_len()
	{
	return outbuf_pos + 1;
	}

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

void MQ_Encoder::flush()// throw (error)
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

void MQ_Encoder::init()
	{
	mq_coder::init();

	A = 0x8000;
	C = 0;
	CT = 12;
	outbuf_pos = -1;
	}

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