#ifndef MQ_DECODER_H
#define	MQ_DECODER_H

#include <vector>

//#include "common/error.h"
#include "mq_coder.hpp"

/**Implementation of the MQ arithmetic decoder as described in the JPEG2000 standard.*/
class mq_decoder: public mq_coder {
public:

	/**Creates a new mq_decoder
	*
	* @throws error if an error occured while reading from the input stream
	*/
	mq_decoder();

	/**Register a new input stream.
	* The internal state is reset to the default values
	*
	* @throws error if an error occured while reading from the input stream
	*/
	void reset(const uint8_t *_buf, const int32_t& max_len);

	/**Decode the next decision bit according to given context
	*
	* @throws error if an error occured while reading from the input stream
	*/
	bool get_bit(const uint8_t& ctx);// throw (error);

protected:

	const uint8_t *inbuf;
	int32_t inbuf_pos, inbuf_max_len;

private:

	uint32_t C, A, CT;

	void init();// throw (error);
	inline bool exchange_mps(const uint8_t& ctx);
	inline bool exchange_lps(const uint8_t& ctx);
	inline void byte_in();// throw (error);
	inline void renorm();// throw (error);
	};

#endif	/* MQ_DECODER_H */