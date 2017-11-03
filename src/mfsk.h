// ----------------------------------------------------------------------------
// mfsk.h  --  mfsk modem
//
// Copyright (C) 2006
//		Dave Freese, W1HKJ
//
// This file is part of fldigi.  Adapted from code contained in gmfsk source code 
// distribution.
//  gmfsk Copyright (C) 2001, 2002, 2003
//  Tomi Manninen (oh2bns@sral.fi)
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _MFSK_H
#define _MFSK_H


#include "globals.h"
#include "modem.h"
#include "filters.h"
#include "interleave.h"
#include "viterbi.h"
#include "complex.h"
#include "mfskvaricode.h"
//#include "mbuffer.h"

#define GET_TX_CHAR_NODATA -1	// no data available
#define GET_TX_CHAR_ETX -3	// end of transmission requested



#define	MFSKSampleRate		8000

// 1 ms per pixel
#define	SAMPLES_PER_PIXEL	8
#define MAX_SYMBOLS			32
#define PICHEADER			64

// NASA coefficients for viterbi encode/decode algorithms

#define	K	7
#define	POLY1	0x6d
#define	POLY2	0x4f

class	mfsk;

struct rxpipe {
	cmplx vector[MAX_SYMBOLS];	//numtones <= 32
};

struct history {
	cmplx val;
	int symnbr;
};
	
class mfsk : public modem {

//#define SCOPESIZE 64

public:
enum {
	TX_STATE_PREAMBLE,
	TX_STATE_START,
	TX_STATE_DATA,
	TX_STATE_END,
	TX_STATE_FLUSH,
	TX_STATE_FINISH,
	TX_STATE_TUNE,
	//TX_STATE_PICTURE_START,
	//TX_STATE_PICTURE
};

enum {
	RX_STATE_DATA,
//	RX_STATE_PICTURE_START,
//	RX_STATE_PICTURE_START_1,
//	RX_STATE_PICTURE_START_2,
//	RX_STATE_PICTURE
};

protected:
// general
	double phaseacc;
	int symlen;
	int symbits;
	int numtones;
	int basetone;
	double tonespacing;
	double basefreq;
	int counter;
	int depth;
// receive
	int				rxstate;
	C_FIR_filter	*hbfilt;
	sfft			*binsfft;
	C_FIR_filter	*bpfilt;
	C_FIR_filter	*xmtfilt;
	//Cmovavg			*vidfilter[SCOPESIZE];
	Cmovavg			*syncfilter;

	viterbi		*dec1;
	viterbi		*dec2;
	interleave	*rxinlv;

	rxpipe		*pipe;
	unsigned int pipeptr;

	unsigned int datashreg;

	//VK2ETA high speed modes
	int	preamble;

	cmplx currvector;
	cmplx prev1vector;
	cmplx prev2vector;

	int currsymbol;
	int prev1symbol;
	int prev2symbol;
	double maxval;
	double prevmaxval;

	double met1;
	double met2;
	//mbuffer<double, 0, 2> scopedata;
	double s2n;
	double sig;
	double noise;
	double afcmetric;
	bool	staticburst;
	
	double currfreq;

	int synccounter;
	int AFC_COUNT;

	unsigned char symbolpair[2];
	int symcounter;
	
	//int RXspp; // samples per pixel
	//int TXspp;

	int symbolbit;

// transmit
	int txstate;
	encoder		*enc;
	interleave	*txinlv;
	unsigned int bitshreg;
	int bitstate;

	void	recvchar(int c);
	void	recvbit(int bit);

// internal processes
	void	decodesymbol(unsigned char symbol);
	void	softdecode(cmplx *bins);
	cmplx	mixer(cmplx in, double f);
	int		harddecode(cmplx *in);
	void	synchronize();
	void	afc();
	void	reset_afc();
	void	eval_s2n();
	void	transmit(double *, int);
	void 	sendsymbol(int sym);
	void	sendbit(int bit);
	void	sendchar(unsigned char c);
	void	sendidle();
	void	flush_xmt_filter(int);
	void	send_prologue();
	void	send_epilogue();
	void	flushtx(int nbits);
	void	clearbits();

	void	s2nreport(void);

public:
	mfsk (trx_mode md);
	~mfsk ();
	void	init();
	void	rx_init();
	void	tx_init();
	void	restart() {};
	int		rx_process(const double *buf, int len);
	int		tx_process();
	void	shutdown();
	int    getsymlen();
};

extern void put_rx_char(unsigned int data);
extern int get_tx_char(void);
#endif
