#include "mq_new.hpp"


MQcoder::MQcoder() {


	B = 0;
	CX = 0;
	D = 0;
	L = 0;

	// initialization  Q_e
	Q_e[0] = 0x5601;
	Q_e[1] = 0x3401;
	Q_e[2] = 0x1801;
	Q_e[3] = 0x0AC1;
	Q_e[4] = 0x0521;
	Q_e[5] = 0x0221;
	Q_e[6] = 0x5601;
	Q_e[7] = 0x5401;
	Q_e[8] = 0x4801;
	Q_e[9] = 0x3801;
	Q_e[10] = 0x3001;
	Q_e[11] = 0x2401;
	Q_e[12] = 0x1C01;
	Q_e[13] = 0x1601;
	Q_e[14] = 0x5601;
	Q_e[15] = 0x5401;
	Q_e[16] = 0x5101;
	Q_e[17] = 0x4801;
	Q_e[18] = 0x3801;
	Q_e[19] = 0x3401;
	Q_e[20] = 0x3001;
	Q_e[21] = 0x2801;
	Q_e[22] = 0x2401;
	Q_e[23] = 0x2201;
	Q_e[24] = 0x1C01;
	Q_e[25] = 0x1801;
	Q_e[26] = 0x1601;
	Q_e[27] = 0x1401;
	Q_e[28] = 0x1201;
	Q_e[29] = 0x1101;
	Q_e[30] = 0x0AC1;
	Q_e[31] = 0x09C1;
	Q_e[32] = 0x08A1;
	Q_e[33] = 0x0521;
	Q_e[34] = 0x0441;
	Q_e[35] = 0x02A1;
	Q_e[36] = 0x0221;
	Q_e[37] = 0x0141;
	Q_e[38] = 0x0111;
	Q_e[39] = 0x0085;
	Q_e[40] = 0x0049;
	Q_e[41] = 0x0025;
	Q_e[42] = 0x0015;
	Q_e[43] = 0x0009;
	Q_e[44] = 0x0005;
	Q_e[45] = 0x0001;
	Q_e[46] = 0x5601;

	// initialization NMPS

	NMPS[0] = 1;
	NMPS[1] = 2;
	NMPS[2] = 3;
	NMPS[3] = 4;
	NMPS[4] = 5;
	NMPS[5] = 38;
	NMPS[6] = 7;
	NMPS[7] = 8;
	NMPS[8] = 9;
	NMPS[9] = 10;
	NMPS[10] = 11;
	NMPS[11] = 12;
	NMPS[12] = 13;
	NMPS[13] = 29;
	NMPS[14] = 15;
	NMPS[15] = 16;
	NMPS[16] = 17;
	NMPS[17] = 18;
	NMPS[18] = 19;
	NMPS[19] = 20;
	NMPS[20] = 21;
	NMPS[21] = 22;
	NMPS[22] = 23;
	NMPS[23] = 24;
	NMPS[24] = 25;
	NMPS[25] = 26;
	NMPS[26] = 27;
	NMPS[27] = 28;
	NMPS[28] = 29;
	NMPS[29] = 30;
	NMPS[30] = 31;
	NMPS[31] = 32;
	NMPS[32] = 33;
	NMPS[33] = 34;
	NMPS[34] = 35;
	NMPS[35] = 36;
	NMPS[36] = 37;
	NMPS[37] = 38;
	NMPS[38] = 39;
	NMPS[39] = 40;
	NMPS[40] = 41;
	NMPS[41] = 42;
	NMPS[42] = 43;
	NMPS[43] = 44;
	NMPS[44] = 45;
	NMPS[45] = 45;
	NMPS[46] = 46;

	// initialization NLPS

	NLPS[0] = 1;
	NLPS[1] = 6;
	NLPS[2] = 9;
	NLPS[3] = 12;
	NLPS[4] = 29;
	NLPS[5] = 33;
	NLPS[6] = 6;
	NLPS[7] = 14;
	NLPS[8] = 14;
	NLPS[9] = 14;
	NLPS[10] = 17;
	NLPS[11] = 18;
	NLPS[12] = 20;
	NLPS[13] = 21;
	NLPS[14] = 14;
	NLPS[15] = 14;
	NLPS[16] = 15;
	NLPS[17] = 16;
	NLPS[18] = 17;
	NLPS[19] = 18;
	NLPS[20] = 19;
	NLPS[21] = 19;
	NLPS[22] = 20;
	NLPS[23] = 21;
	NLPS[24] = 22;
	NLPS[25] = 23;
	NLPS[26] = 24;
	NLPS[27] = 25;
	NLPS[28] = 26;
	NLPS[29] = 27;
	NLPS[30] = 28;
	NLPS[31] = 29;
	NLPS[32] = 30;
	NLPS[33] = 31;
	NLPS[34] = 32;
	NLPS[35] = 33;
	NLPS[36] = 34;
	NLPS[37] = 35;
	NLPS[38] = 36;
	NLPS[39] = 37;
	NLPS[40] = 38;
	NLPS[41] = 39;
	NLPS[42] = 40;
	NLPS[43] = 41;
	NLPS[44] = 42;
	NLPS[45] = 43;
	NLPS[46] = 46;

	// initialization SWITCH

	SWITCH[0] = 1;
	SWITCH[1] = 0;
	SWITCH[2] = 0;
	SWITCH[3] = 0;
	SWITCH[4] = 0;
	SWITCH[5] = 0;
	SWITCH[6] = 1;
	SWITCH[7] = 0;
	SWITCH[8] = 0;
	SWITCH[9] = 0;
	SWITCH[10] = 0;
	SWITCH[11] = 0;
	SWITCH[12] = 0;
	SWITCH[13] = 0;
	SWITCH[14] = 1;
	SWITCH[15] = 0;
	SWITCH[16] = 0;
	SWITCH[17] = 0;
	SWITCH[18] = 0;
	SWITCH[19] = 0;
	SWITCH[20] = 0;
	SWITCH[21] = 0;
	SWITCH[22] = 0;
	SWITCH[23] = 0;
	SWITCH[24] = 0;
	SWITCH[25] = 0;
	SWITCH[26] = 0;
	SWITCH[27] = 0;
	SWITCH[28] = 0;
	SWITCH[29] = 0;
	SWITCH[30] = 0;
	SWITCH[31] = 0;
	SWITCH[32] = 0;
	SWITCH[33] = 0;
	SWITCH[34] = 0;
	SWITCH[35] = 0;
	SWITCH[36] = 0;
	SWITCH[37] = 0;
	SWITCH[38] = 0;
	SWITCH[39] = 0;
	SWITCH[40] = 0;
	SWITCH[41] = 0;
	SWITCH[42] = 0;
	SWITCH[43] = 0;
	SWITCH[44] = 0;
	SWITCH[45] = 0;
	SWITCH[46] = 0;
	}


MQcoder::~MQcoder() {}

void MQcoder::Initialization() {
	/* initialization */
	A = 0x00008000;
	C = 0x00000000;
	// BP = BPST -1;
	CT = 12;
	L = -1;

	if(B == 0xFF)
		CT = 13;
	/*reset INDEX[CX] and MPS[CX] with their initial values*/
	for(int i = 0; i < 19; ++i)
		{
		if(i == 0)
			{
			INDEX[i] = 4;
			}
		else if(i == 17) {
			INDEX[i] = 3;
			}
		else if(i == 18) {
			INDEX[i] = 46;
			}
		else {
			INDEX[i] = 0;
			}

		MPS[i] = 0;
		}

	}

void MQcoder::push_B(std::queue <uint8_t> *bit_stream) {
	if(L >= 0) {
		bit_stream->push(B);
		}
	L = L + 1; // length (in bytes) of the bitstream, incrementation
	}

void MQcoder::bit_Stuffing(std::queue <uint8_t> *bit_stream) {
	// BP = BP + 1; // OUTPUT B
	push_B(bit_stream);
	B = C >> 20; // "cbbb bbbb" bit of C
	C = C & 0x000FFFFF;
	CT = 7;
	}

void MQcoder::no_bit_Stuffing(std::queue <uint8_t> *bit_stream) {
	// BP = BP + 1; // OUTPUT B
	push_B(bit_stream);
	B = C >> 19; // "bbb bbbb b" bit of C
	C = C & 0x0007FFFF;
	CT = 8;
	}

void MQcoder::ByteOut(std::queue <uint8_t> *bit_stream) {
	if(B == 0xFF) {
		bit_Stuffing(bit_stream);
		}
	else {
		if(C < 0x08000000) {     // no carry bit
			no_bit_Stuffing(bit_stream);
			}
		else {
			B = B + 1; // add carry bit to B
			if(B == 0xFF) {
				C = C & 0x07FFFFFF; //set 5 MSBs of C to zero while keeping other same
				bit_Stuffing(bit_stream);
				}
			else {
				no_bit_Stuffing(bit_stream);
				}

			}
		}
	}

void MQcoder::RenormalizationENC(std::queue <uint8_t> *bit_stream) {
	do {
		A = A << 1;
		C = C << 1;
		CT = CT - 1;
		if(CT == 0) {
			ByteOut(bit_stream);
			}
		} while(A < 0x00008000);
	}

void MQcoder::CodeMPS(std::queue <uint8_t> *bit_stream) {
	index = INDEX[CX];
	qe = Q_e[index];
	A = A - qe;

	if(A < 0x00008000) {
		if(A < qe) { // condition exchange
			A = qe;
			}
		else {
			C = C + qe;
			}
		/* choose next index for MPS */
		INDEX[CX] = NMPS[index];

		RenormalizationENC(bit_stream);
		}
	else {
		C = C + qe;
		}

	}

void MQcoder::CodeLPS(std::queue <uint8_t> *bit_stream) {
	index = INDEX[CX];
	qe = Q_e[index];
	A = A - qe;

	if(A >= qe) {
		A = qe;
		}
	else {
		C = C + qe;
		}

	if(SWITCH[index] == 1) {
		/* change the sense of MPS(CX) */
		MPS[CX] = 1 - MPS[CX];
		}
	/* choose next index for LPS */
	INDEX[CX] = NLPS[index];

	RenormalizationENC(bit_stream);

	}

void MQcoder::FLUSHregister(std::queue <uint8_t> *bit_stream) {
	int TempC = C + A;
	C = C | 0x0000FFFF;
	if(C > TempC) {
		C = C - 0x00008000;
		}
	C = C << CT;
	ByteOut(bit_stream);
	C = C << CT;
	ByteOut(bit_stream);

	if(B == 0xFF) {
		B = 0x00;				// discard B 
		}
	else {
		// BP = BP + 1; // output B
		ByteOut(bit_stream);
		}
	}

void MQcoder::run(std::queue <int> _CX, std::queue<int> _D, std::queue <int> *headerqueue, std::queue <uint8_t> *bit_stream) {

	//context pair;

	int counter = 0;
	do {
		Initialization();
		// read (CX,D) pair
		if(!_CX.empty()) {
			do {
				// read (CX,D) pair
				//pair = localqueue->front();
				if(_CX.empty()) {
					break;
				}
				//std::cout << _CX.size() << std::endl;
				CX = _CX.front();
				D = _D.front();
				//std::cout << CX << std::endl;
				counter++;
				_CX.pop();
				_D.pop();
				if(counter > 100) {
					// system("PAUSE");
					}

				if(D >= 0) {
					if(D == MPS[CX]) {
						CodeMPS(bit_stream);
						}
					else {
						CodeLPS(bit_stream);

						}
					}
				} while(D >= 0);// code block seperation is identified when D < 0
				if(L>0) {
					FLUSHregister(bit_stream);
					}
				else {
					L = 0;
					}
				// pushes to headerqueue
				headerqueue->push(CX); // no:of zero bit planes 
				headerqueue->push(-D - 1); // no of coding passes (D = -(no:of codingPasses + 1) )
				headerqueue->push(L); // no:of bytes of the compressed bitstream
			}
		} while(!(_CX.empty()));
	}