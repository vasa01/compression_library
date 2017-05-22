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
 * @file	block_coder.cpp
 *
 * @brief	Implements block coder init table .
 */

#include "block_coder.hpp"
/**
 * static_initializer_t
 */
block_coder::static_initializer_t::static_initializer_t()
	{
	//zero coding contexts
	// LL and LH
	for(uint16_t i = 0; i < 0xFF; ++i) {

		uint8_t h = Tool::PopCount(uint16_t(i & (SIG_W | SIG_E)));
		uint8_t v = Tool::PopCount(uint16_t(i & (SIG_N | SIG_S)));
		uint8_t d = Tool::PopCount(uint16_t((i & (SIG_NW | SIG_NE | SIG_SW | SIG_SE))));

		uint8_t ctx = 0;
		if(h == 0) {
			if(v == 0) {
				if(d == 0) {
					ctx = 0;
					}
				else if(d == 1) {
					ctx = 1;
					}
				else {
					ctx = 2;
					}
				}
			else if(v == 1) {
				ctx = 3;
				}
			else {
				ctx = 4;
				}
			}
		else if(h == 1) {
			if(v == 0) {
				if(d == 0) {
					ctx = 5;
					}
				else {
					ctx = 6;
					}
				}
			else {
				ctx = 7;
				}
			}
		else {
			ctx = 8;
			}
		lut_zero_context[LL][i] = lut_zero_context[LH][i] = ctx;
		}

	// HL
	for(int i = 0; i < 256; ++i) {

		uint8_t h = Tool::PopCount(uint16_t(i & (SIG_W | SIG_E)));
		uint8_t v = Tool::PopCount(uint16_t(i & (SIG_N | SIG_S)));
		uint8_t d = Tool::PopCount(uint16_t(i & (SIG_NW | SIG_NE | SIG_SW | SIG_SE)));
		uint8_t ctx = 0;

		if(v == 0) {
			if(h == 0) {
				if(d == 0) {
					ctx = 0;
					}
				else if(d == 1) {
					ctx = 1;
					}
				else {
					ctx = 2;
					}
				}
			else if(h == 1) {
				ctx = 3;
				}
			else {
				ctx = 4;
				}
			}
		else if(v == 1) {
			if(h == 0) {
				if(d == 0) {
					ctx = 5;
					}
				else {
					ctx = 6;
					}
				}
			else {
				ctx = 7;
				}
			}
		else {
			ctx = 8;
			}

		lut_zero_context[HL][i] = ctx;
		}

	// HH
	for(int i = 0; i < 256; ++i) {

		uint8_t hv = Tool::PopCount(uint16_t(i & (SIG_FOUR)));
		uint8_t d = Tool::PopCount(uint16_t(i & (SIG_NW | SIG_NE | SIG_SW | SIG_SE)));

		uint8_t ctx = 0;
		if(d == 0) {
			if(hv == 0) {
				ctx = 0;
				}
			else if(hv == 1) {
				ctx = 1;
				}
			else {
				ctx = 2;
				}
			}
		else if(d == 1) {
			if(hv == 0) {
				ctx = 3;
				}
			else if(hv == 1) {
				ctx = 4;
				}
			else {
				ctx = 5;
				}
			}
		else if(d == 2) {
			if(hv == 0) {
				ctx = 6;
				}
			else {
				ctx = 7;
				}
			}
		else {
			ctx = 8;
			}
		lut_zero_context[HH][i] = ctx;
		}

	//sign coding context and sign prediction
	for(int i = 0; i < 256; ++i) {
		uint8_t ctx = 0;
		bool prediction = 0;

		uint16_t state = i << 4;
		int8_t hc
			= std::min(((state & (SIG_E | SGN_E)) == SIG_E) + ((state & (SIG_W | SGN_W)) == SIG_W), 1)
			- std::min(((state & (SIG_E | SGN_E)) == (SIG_E | SGN_E)) + ((state & (SIG_W | SGN_W)) == (SIG_W | SGN_W)), 1);

		int8_t vc
			= std::min(((state & (SIG_N | SGN_N)) == SIG_N) + ((state & (SIG_S | SGN_S)) == SIG_S), 1)
			- std::min(((state & (SIG_N | SGN_N)) == (SIG_N | SGN_N)) + ((state & (SIG_S | SGN_S)) == (SIG_S | SGN_S)), 1);

		if(hc == 1) {
			if(vc == -1) {
				ctx = 11;
				}
			else if(vc == 0) {
				ctx = 12;
				}
			else {
				ctx = 13;
				}
			}
		else if(hc == 0) {
			if(vc == -1) {
				ctx = 10;
				prediction = true;
				}
			else if(vc == 0) {
				ctx = 9;
				}
			else {
				ctx = 10;
				}
			}
		else {
			prediction = true;
			if(vc == -1) {
				ctx = 13;
				}
			else if(vc == 0) {
				ctx = 12;
				}
			else {
				ctx = 11;
				}
			}
		lut_sign_context[i] = ctx;
		lut_sign_prediction[i] = prediction;
		}
	}

bool block_coder::lut_sign_prediction[256] = { 0 };
uint8_t block_coder::lut_sign_context[256] = { 0 };
uint8_t block_coder::lut_zero_context[4][256] = {
		{ 0 }
	};
