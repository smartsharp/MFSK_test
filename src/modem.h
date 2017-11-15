// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of fldigi
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef	_MODEM_H
#define	_MODEM_H

#include <string>


#include "globals.h"
#include "ascii.h"

#define	OUTBUFSIZE	16384
// Constants for signal searching & s/n threshold
#define SIGSEARCH 5

#define GET_TX_CHAR_NODATA 0xffff	// no data available
#define GET_TX_CHAR_ETX 0xfffe	// end of transmission requested


#define TWOPI (2.0 * M_PI)

#define TX_IN_BUF_SIZE 512
#define RX_OUT_BUF_SIZE 512


typedef void (*func_on_tx_write)(double *buf, int len);
typedef int (*func_on_rx_read)(double **buf, int *len);


class modem {
public:
	static double	frequency;
	static double	tx_frequency;
	static bool	freqlock;
	static unsigned long tx_sample_count;
	static unsigned int tx_sample_rate;
protected:
	trx_mode mode;

	int		fragmentsize;
	int		samplerate;
	bool	reverse;
	int		sigsearch;

	double	bandwidth;
	double	freqerr;
	double	rx_corr;
	double	tx_corr;

	double 	squelch;
	double	metric;
	double	syncpos;

	int	backspaces;
	unsigned char *txstr;
	unsigned char *txptr;

	double outbuf[OUTBUFSIZE];


	int scptr;
	unsigned cap;

public:
	modem();
	virtual ~modem(){};

// these processes must be declared in the derived class
	virtual void init();
	virtual void tx_init () = 0;
	virtual void rx_init () = 0;
	virtual void restart () = 0;
	virtual void rx_flush() {};
	virtual int  tx_process () = 0;
	virtual int  rx_process (const double *, int len) = 0;
	virtual void shutdown(){};
	virtual void set1(int, int){};
	virtual void set2(int, int){};
	virtual void makeTxViewer(int W, int H){};

	virtual void	searchDown() {};
	virtual void	searchUp() {};

	/// Inlined const getters are faster and smaller.
	trx_mode	get_mode() const { return mode; };
	const char	*get_mode_name() const { return mode_info[get_mode()].sname;}
	unsigned int iface_io() const { return mode_info[get_mode()].iface_io;}
	virtual void	set_freq(double);
	/// Inlining small formulas is still faster and shorter.
	int		get_freq() const { return (int)( frequency + 0.5 ); }
	void		init_freqlock();
	void		set_freqlock(bool);
	void		set_sigsearch(int n) { sigsearch = n; freqerr = 0.0;};
	bool		freqlocked() const { return freqlock;}
	/// Getters are semantically const.
	double		get_txfreq() const;
	double		get_txfreq_woffset() const;
	void		set_metric(double);
	void		display_metric(double);
	double		get_metric() const { return metric;}
	void		set_reverse(bool on);
	bool		get_reverse() const { return reverse;}
	double		get_bandwidth() const { return bandwidth;}
	void		set_bandwidth(double);
	int		get_samplerate() const { return samplerate;}
	void		set_samplerate(int);
	void		init_queues();

	void		ModulateXmtr(double *, int);


	void		set_stopflag(bool b) { stopflag = b;};
	bool		get_stopflag() const { return stopflag; };

	unsigned	get_cap(void) const { return cap; }
	enum { CAP_AFC = 1 << 0, CAP_AFC_SR = 1 << 1, CAP_REV = 1 << 2,
	       CAP_IMG = 1 << 3, CAP_BW = 1 << 4, CAP_RX = 1 << 5,
	       CAP_TX = 1 << 6
	};


	virtual int    getbuflen();

// JD & DF for multiple carriers
public:
	int  numcarriers; //Number of parallel carriers for M CAR PSK and PSKR and QPSKR
	int  symbols; //JD for multiple carriers
	int  acc_symbols;
	int  char_symbols;
	int  xmt_symbols;
	int  ovhd_symbols;
	int  acc_samples;
	int  char_samples;
	int  xmt_samples;
	int  ovhd_samples;


public:
	bool	stopflag;
	uint8_t tx_in_buf[TX_IN_BUF_SIZE+1];
	int tx_in_len;
	int tx_in_idx;

	uint8_t rx_out_buf[RX_OUT_BUF_SIZE+1];
	int rx_out_len;
	int rx_out_idx;

	func_on_tx_write on_tx_write;
	func_on_rx_read on_rx_read;

	bool afconoff;
	bool sqlonoff;
	bool pskpilot;
	bool psk8DCDShortFlag;
	double pilot_power;
};

extern modem *null_modem;

extern modem *mfsk8_modem;
extern modem *mfsk16_modem;
extern modem *mfsk32_modem;
// experimental modes
extern modem *mfsk4_modem;
extern modem *mfsk11_modem;
extern modem *mfsk22_modem;
extern modem *mfsk31_modem;
extern modem *mfsk64_modem;
extern modem *mfsk128_modem;
extern modem *mfsk64l_modem;
extern modem *mfsk128l_modem;

extern modem *psk31_modem;
extern modem *psk63_modem;
extern modem *psk63f_modem;
extern modem *psk125_modem;
extern modem *psk250_modem;
extern modem *psk500_modem;
extern modem *psk1000_modem;

extern modem *qpsk31_modem;
extern modem *qpsk63_modem;
extern modem *qpsk125_modem;
extern modem *qpsk250_modem;
extern modem *qpsk500_modem;

extern modem *_8psk125_modem;
extern modem *_8psk250_modem;
extern modem *_8psk500_modem;
extern modem *_8psk1000_modem;
extern modem *_8psk1200_modem;
extern modem *_8psk1333_modem;

extern modem *_8psk125fl_modem;
extern modem *_8psk125f_modem;
extern modem *_8psk250fl_modem;
extern modem *_8psk250f_modem;
extern modem *_8psk500f_modem;
extern modem *_8psk1000f_modem;
extern modem *_8psk1200f_modem;
extern modem *_8psk1333f_modem;

extern modem *psk125r_modem;
extern modem *psk250r_modem;
extern modem *psk500r_modem;
extern modem *psk1000r_modem;

extern modem *psk800_c2_modem;
extern modem *psk800r_c2_modem;

extern modem *psk1000_c2_modem;
extern modem *psk1000r_c2_modem;

extern modem *psk63r_c4_modem;
extern modem *psk63r_c5_modem;
extern modem *psk63r_c10_modem;
extern modem *psk63r_c20_modem;
extern modem *psk63r_c32_modem;

extern modem *psk125r_c4_modem;
extern modem *psk125r_c5_modem;
extern modem *psk125r_c10_modem;
extern modem *psk125_c12_modem;
extern modem *psk125r_c12_modem;
extern modem *psk125r_c16_modem;

extern modem *psk250r_c2_modem;
extern modem *psk250r_c3_modem;
extern modem *psk250r_c5_modem;
extern modem *psk250_c6_modem;
extern modem *psk250r_c6_modem;
extern modem *psk250r_c7_modem;

extern modem *psk500_c2_modem;
extern modem *psk500_c4_modem;

extern modem *psk500r_c2_modem;
extern modem *psk500r_c3_modem;
extern modem *psk500r_c4_modem;

extern modem *olivia_modem;
extern modem *olivia_4_250_modem;
extern modem *olivia_8_250_modem;
extern modem *olivia_4_500_modem;
extern modem *olivia_8_500_modem;
extern modem *olivia_16_500_modem;
extern modem *olivia_8_1000_modem;
extern modem *olivia_16_1000_modem;
extern modem *olivia_32_1000_modem;
extern modem *olivia_64_2000_modem;



#endif
