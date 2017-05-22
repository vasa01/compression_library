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
 * @file	mq_coder.hpp
 *
 * @brief	mq_coder implementation.
 */

#include "mq_coder.hpp"

/**
 * init
 */
void mq_coder::init()
	{
	state_mapping.resize(19);
	state_mapping[0] = &states[2 * 4];
	for(int i = 1; i < 17; ++i) {
		state_mapping[i] = &states[0];
		}
	state_mapping[17] = &states[2 * 3];
	state_mapping[18] = &states[2 * 46];
	}

mq_coder::state_t mq_coder::states[2 * 47] = {
		{ 0x5601, 0, &states[2], &states[3] },
		{ 0x5601, 1, &states[3], &states[2] },
		{ 0x3401, 0, &states[4], &states[12] },
		{ 0x3401, 1, &states[5], &states[13] },
		{ 0x1801, 0, &states[6], &states[18] },
		{ 0x1801, 1, &states[7], &states[19] },
		{ 0x0ac1, 0, &states[8], &states[24] },
		{ 0x0ac1, 1, &states[9], &states[25] },
		{ 0x0521, 0, &states[10], &states[58] },
		{ 0x0521, 1, &states[11], &states[59] },
		{ 0x0221, 0, &states[76], &states[66] },
		{ 0x0221, 1, &states[77], &states[67] },
		{ 0x5601, 0, &states[14], &states[13] },
		{ 0x5601, 1, &states[15], &states[12] },
		{ 0x5401, 0, &states[16], &states[28] },
		{ 0x5401, 1, &states[17], &states[29] },
		{ 0x4801, 0, &states[18], &states[28] },
		{ 0x4801, 1, &states[19], &states[29] },
		{ 0x3801, 0, &states[20], &states[28] },
		{ 0x3801, 1, &states[21], &states[29] },
		{ 0x3001, 0, &states[22], &states[34] },
		{ 0x3001, 1, &states[23], &states[35] },
		{ 0x2401, 0, &states[24], &states[36] },
		{ 0x2401, 1, &states[25], &states[37] },
		{ 0x1c01, 0, &states[26], &states[40] },
		{ 0x1c01, 1, &states[27], &states[41] },
		{ 0x1601, 0, &states[58], &states[42] },
		{ 0x1601, 1, &states[59], &states[43] },
		{ 0x5601, 0, &states[30], &states[29] },
		{ 0x5601, 1, &states[31], &states[28] },
		{ 0x5401, 0, &states[32], &states[28] },
		{ 0x5401, 1, &states[33], &states[29] },
		{ 0x5101, 0, &states[34], &states[30] },
		{ 0x5101, 1, &states[35], &states[31] },
		{ 0x4801, 0, &states[36], &states[32] },
		{ 0x4801, 1, &states[37], &states[33] },
		{ 0x3801, 0, &states[38], &states[34] },
		{ 0x3801, 1, &states[39], &states[35] },
		{ 0x3401, 0, &states[40], &states[36] },
		{ 0x3401, 1, &states[41], &states[37] },
		{ 0x3001, 0, &states[42], &states[38] },
		{ 0x3001, 1, &states[43], &states[39] },
		{ 0x2801, 0, &states[44], &states[38] },
		{ 0x2801, 1, &states[45], &states[39] },
		{ 0x2401, 0, &states[46], &states[40] },
		{ 0x2401, 1, &states[47], &states[41] },
		{ 0x2201, 0, &states[48], &states[42] },
		{ 0x2201, 1, &states[49], &states[43] },
		{ 0x1c01, 0, &states[50], &states[44] },
		{ 0x1c01, 1, &states[51], &states[45] },
		{ 0x1801, 0, &states[52], &states[46] },
		{ 0x1801, 1, &states[53], &states[47] },
		{ 0x1601, 0, &states[54], &states[48] },
		{ 0x1601, 1, &states[55], &states[49] },
		{ 0x1401, 0, &states[56], &states[50] },
		{ 0x1401, 1, &states[57], &states[51] },
		{ 0x1201, 0, &states[58], &states[52] },
		{ 0x1201, 1, &states[59], &states[53] },
		{ 0x1101, 0, &states[60], &states[54] },
		{ 0x1101, 1, &states[61], &states[55] },
		{ 0x0ac1, 0, &states[62], &states[56] },
		{ 0x0ac1, 1, &states[63], &states[57] },
		{ 0x09c1, 0, &states[64], &states[58] },
		{ 0x09c1, 1, &states[65], &states[59] },
		{ 0x08a1, 0, &states[66], &states[60] },
		{ 0x08a1, 1, &states[67], &states[61] },
		{ 0x0521, 0, &states[68], &states[62] },
		{ 0x0521, 1, &states[69], &states[63] },
		{ 0x0441, 0, &states[70], &states[64] },
		{ 0x0441, 1, &states[71], &states[65] },
		{ 0x02a1, 0, &states[72], &states[66] },
		{ 0x02a1, 1, &states[73], &states[67] },
		{ 0x0221, 0, &states[74], &states[68] },
		{ 0x0221, 1, &states[75], &states[69] },
		{ 0x0141, 0, &states[76], &states[70] },
		{ 0x0141, 1, &states[77], &states[71] },
		{ 0x0111, 0, &states[78], &states[72] },
		{ 0x0111, 1, &states[79], &states[73] },
		{ 0x0085, 0, &states[80], &states[74] },
		{ 0x0085, 1, &states[81], &states[75] },
		{ 0x0049, 0, &states[82], &states[76] },
		{ 0x0049, 1, &states[83], &states[77] },
		{ 0x0025, 0, &states[84], &states[78] },
		{ 0x0025, 1, &states[85], &states[79] },
		{ 0x0015, 0, &states[86], &states[80] },
		{ 0x0015, 1, &states[87], &states[81] },
		{ 0x0009, 0, &states[88], &states[82] },
		{ 0x0009, 1, &states[89], &states[83] },
		{ 0x0005, 0, &states[90], &states[84] },
		{ 0x0005, 1, &states[91], &states[85] },
		{ 0x0001, 0, &states[90], &states[86] },
		{ 0x0001, 1, &states[91], &states[87] },
		{ 0x5601, 0, &states[92], &states[92] },
		{ 0x5601, 1, &states[93], &states[93] },
	};
