#ifndef MQ_H 
#define MQ_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <queue>

#include <bitset>



class MQcoder
	{
	public:
		MQcoder();
		~MQcoder();
		void run(std::queue<int> cx, std::queue<int> d, std::queue <int> *headerqueue, std::queue<uint8_t> *bit_stream);
	private:
		//----------global constants-------------------------------------------------------------------------------------

		unsigned int Q_e[47]; // probabilty estimation lookup table

		int MPS[19];

		unsigned int NMPS[47];

		unsigned int NLPS[47];

		bool SWITCH[47];

		// The initial index of context for BAC probability estimation lookup table
		int INDEX[19];
		//--------------global variables--------------------------------------------------
		int  A; // interval register
		int  C; // code register
		int  qe; // current probability value of index
		int  index; // current index of the CX
		int  BP; // compressed data buffer pointer
		int  BPST; // pointer which points to where the first byte is going to be placed
		int  CT; // counter for no: of shifts applied on A and C
		uint8_t B; // (output) byte pointed by the BP
		int  CX; // this is an input
		int D; // this is an input
		int L; // length(in bytes) of the bitstream for a given code block
			   // vector<char> bit_stream; // vector to store the bit stream

			   //--------------function prototypes--------------------------------------------------
		void Initialization();
		void push_B(std::queue <uint8_t> *bit_stream);
		void bit_Stuffing(std::queue <uint8_t> *bit_stream);
		void no_bit_Stuffing(std::queue <uint8_t> *bit_stream);
		void ByteOut(std::queue <uint8_t> *bit_stream);
		void RenormalizationENC(std::queue <uint8_t> *bit_stream);
		void CodeMPS(std::queue <uint8_t> *bit_stream);
		void CodeLPS(std::queue <uint8_t> *bit_stream);
		void FLUSHregister(std::queue <uint8_t> *bit_stream);

	};

#endif
