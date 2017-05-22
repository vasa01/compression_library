#ifndef BLOCK_CODER_H
#define	BLOCK_CODER_H

//#include <SDL/SDL_stdinc.h>
//
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "tool.hpp"

/**Base class for a JPEG2000 block coder (T1 coder)*/
class block_coder {
public:

	/**Subband of a Discrete Wavelet Transform*/
	enum subband_t {
		LL = 0,
		LH,
		HL,
		HH,
		NUM_OF_SUBBANDS
		};

protected:

	block_coder() { }

	/**The per-sample state flags
	* @attention The order is important for the sign coding context lut!
	*/
	enum FLAGS {
		//
		/**Northeast neighbor was found to be significant*/
		SIG_NE = 0x0001,
		/**Southeast neighbor was found to be significant*/
		SIG_SE = 0x0002,
		/**Southwest neighbor was found to be significant*/
		SIG_SW = 0x0004,
		/**Northwest neighbor was found to be significant*/
		SIG_NW = 0x0008,
		/**North neighbor was found to be significant*/
		SIG_N = 0x0010,
		/**South neighbor was found to be significant*/
		SIG_S = 0x0020,
		/**East neighbor was found to be significant*/
		SIG_E = 0x0040,
		/**West neighbor was found to be significant*/
		SIG_W = 0x0080,
		/**The four neighbors in the Neumann neighborhood are found to be significant*/
		SIG_FOUR = (SIG_N | SIG_E | SIG_S | SIG_W),
		/**The eight neighbors in the Moore neighborhood are found to be significant*/
		SIG_EIGHT = (SIG_FOUR | SIG_NE | SIG_SE | SIG_SW | SIG_NW),
		/**North neighbor is negative*/
		SGN_N = 0x0100,
		/**South neighbor is negative*/
		SGN_S = 0x0200,
		/**East neighbor is negative*/
		SGN_E = 0x0400,
		/**West neighbor is negative*/
		SGN_W = 0x0800,
		/**The four neighbors in the Neumann neighborhood are negative*/
		SGN_FOUR = (SGN_N | SGN_S | SGN_E | SGN_W),
		/**The sample was found to be significant*/
		PASS_SIG = 0x1000,
		/**Magnitude refinement has been applied to this sample*/
		PASS_REF = 0x2000,
		/**This sample was already processed in significance pass*/
		PASS_VISIT = 0x4000
		};

	/**Static look-up-table for the sign prediction in sign coding*/
	static bool lut_sign_prediction[256];

	/**Staic look-up-table for the context in sign coding*/
	static uint8_t lut_sign_context[256];

	/**Staic look-up-table for the context in zero coding*/
	static uint8_t lut_zero_context[4][256];

private:

	/**Initializes the static look-up-tables*/
	struct static_initializer_t {
		static_initializer_t();
		} static_initializer;
	};

#endif	/* BLOCK_CODER_H */