#ifndef MQ_CODING_H
#define	MQ_CODING_H

#include <vector>

//#include "common/error.h"
#include "mq_coder.hpp"
#include <iostream>

/** Implementation of the MQ arithmetic coder as described in the JPEG2000 standard.*/
class MQ_Encoder: public mq_coder {
public:

	/**Creates a new MQ_Encoder*/
	MQ_Encoder();

	/**Creates a new MQ_Encoder*/
	MQ_Encoder(uint8_t *_buf, const int32_t& max_len);

	/** todo
	* Remember to call flush() before all intermediate data gets deleted!
	*/
	void reset();

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