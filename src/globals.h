// ----------------------------------------------------------------------------
// globals.h  --  constants, variables, arrays & functions that need to be
//                  outside of any thread
//
// Copyright (C) 2006-2007
//		Dave Freese, W1HKJ
// Copyright (C) 2007-2010
//		Stelios Bounanos, M0GLD
//
// This file is part of fldigi.  Adapted in part from code contained in gmfsk
// source code distribution.
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

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdint.h>
#include <string>

enum {DISABLED_IO, ARQ_IO, KISS_IO, XMLRPC_IO, FLRIG_IO, FLLOG_IO};

#define DATA_IO_NA  0x00
#define DATA_IO_TCP 0x01
#define DATA_IO_UDP 0x02


enum state_t {
	STATE_PAUSE = 0,
	STATE_RX,
	STATE_TX,
	STATE_RESTART,
	STATE_TUNE,
	STATE_ABORT,
	STATE_FLUSH,
	STATE_NOOP,
	STATE_EXIT,
	STATE_ENDED,
	STATE_IDLE,
	STATE_NEW_MODEM
};

enum {
	MODE_PREV = -2,
	MODE_NEXT,

	MODE_NULL,


	MODE_MFSK8,
	MODE_MFSK16,
	MODE_MFSK32,
	MODE_MFSK4,
	MODE_MFSK11,
	MODE_MFSK22,
	MODE_MFSK31,
	MODE_MFSK64,
	MODE_MFSK128,
	MODE_MFSK64L,
	MODE_MFSK128L,
	MODE_MFSK_FIRST = MODE_MFSK8,
	MODE_MFSK_LAST = MODE_MFSK128L,

	MODE_PSK31,
	MODE_PSK63,
	MODE_PSK63F,
	MODE_PSK125,
	MODE_PSK250,
	MODE_PSK500,
	MODE_PSK1000,

	MODE_12X_PSK125,
	MODE_6X_PSK250,
	MODE_2X_PSK500,
	MODE_4X_PSK500,
	MODE_2X_PSK800,
	MODE_2X_PSK1000,

	MODE_PSK_FIRST = MODE_PSK31,
	MODE_PSK_LAST = MODE_2X_PSK1000,

	MODE_QPSK31,
	MODE_QPSK63,
	MODE_QPSK125,
	MODE_QPSK250,
	MODE_QPSK500,

	MODE_QPSK_FIRST = MODE_QPSK31,
	MODE_QPSK_LAST = MODE_QPSK500,

	MODE_8PSK125,
	MODE_8PSK125FL,
	MODE_8PSK125F,
	MODE_8PSK250,
	MODE_8PSK250FL,
	MODE_8PSK250F,
	MODE_8PSK500,
	MODE_8PSK500F,
	MODE_8PSK1000,
	MODE_8PSK1000F,
	MODE_8PSK1200F,
	MODE_8PSK_FIRST = MODE_8PSK125,
	MODE_8PSK_LAST = MODE_8PSK1200F,

//	MODE_PACKET,
// high speed && multiple carrier modes

	MODE_PSK125R,
	MODE_PSK250R,
	MODE_PSK500R,
	MODE_PSK1000R,

	MODE_4X_PSK63R,
	MODE_5X_PSK63R,
	MODE_10X_PSK63R,
	MODE_20X_PSK63R,
	MODE_32X_PSK63R,

	MODE_4X_PSK125R,
	MODE_5X_PSK125R,
	MODE_10X_PSK125R,
	MODE_12X_PSK125R,
	MODE_16X_PSK125R,

	MODE_2X_PSK250R,
	MODE_3X_PSK250R,
	MODE_5X_PSK250R,
	MODE_6X_PSK250R,
	MODE_7X_PSK250R,

	MODE_2X_PSK500R,
	MODE_3X_PSK500R,
	MODE_4X_PSK500R,

	MODE_2X_PSK800R,
	MODE_2X_PSK1000R,

	MODE_PSKR_FIRST = MODE_PSK125R,
	MODE_PSKR_LAST = MODE_2X_PSK1000R,

	MODE_OLIVIA,
	MODE_OLIVIA_4_250,
	MODE_OLIVIA_8_250,
	MODE_OLIVIA_4_500,
	MODE_OLIVIA_8_500,
	MODE_OLIVIA_16_500,
	MODE_OLIVIA_8_1000,
	MODE_OLIVIA_16_1000,
	MODE_OLIVIA_32_1000,
	MODE_OLIVIA_64_2000,
	MODE_OLIVIA_FIRST = MODE_OLIVIA,
	MODE_OLIVIA_LAST = MODE_OLIVIA_64_2000,



	NUM_MODES,
	NUM_RXTX_MODES = NUM_MODES - 2
};

typedef intptr_t trx_mode;

struct mode_info_t {
	trx_mode mode;
	class modem **modem;
	const char *sname;
	const char *name;
	const char *pskmail_name;
	const char *adif_name;
	const char *vid_name;
	const unsigned int iface_io; // Some modes are not usable for a given interface.
};
extern const struct mode_info_t mode_info[NUM_MODES];

class qrg_mode_t
{
public:
	long long rfcarrier;
	std::string rmode;
	int carrier;
	trx_mode mode;
	std::string usage;

	qrg_mode_t() :
		rfcarrier(0),
		rmode("NONE"),
		carrier(0),
		mode(NUM_MODES),
		usage("") { }
	qrg_mode_t(long long rfc_, std::string rm_, int c_, trx_mode m_, std::string use_ = "")
                : rfcarrier(rfc_), rmode(rm_), carrier(c_), mode(m_), usage(use_) { }
	bool operator<(const qrg_mode_t& rhs) const
        {
		return rfcarrier < rhs.rfcarrier;
	}
	bool operator==(const qrg_mode_t& rhs) const
	{
		return rfcarrier == rhs.rfcarrier && rmode == rhs.rmode &&
		       carrier == rhs.carrier && mode == rhs.mode;
	}
	std::string str(void);
};
std::ostream& operator<<(std::ostream& s, const qrg_mode_t& m);
std::istream& operator>>(std::istream& s, qrg_mode_t& m);

#include <bitset>
class mode_set_t : public std::bitset<NUM_MODES> {};

enum band_t {
	BAND_160M, BAND_80M, BAND_75M, BAND_60M, BAND_40M, BAND_30M, BAND_20M,
	BAND_17M, BAND_15M, BAND_12M, BAND_10M, BAND_6M, BAND_4M, BAND_2M, BAND_125CM,
	BAND_70CM, BAND_33CM, BAND_23CM, BAND_13CM, BAND_9CM, BAND_6CM, BAND_3CM, BAND_125MM,
	BAND_6MM, BAND_4MM, BAND_2P5MM, BAND_2MM, BAND_1MM, BAND_OTHER, NUM_BANDS
};

band_t band(long long freq_hz);
band_t band(const char* freq_mhz);
const char* band_name(band_t b);
const char* band_name(const char* freq_mhz);
const char* band_freq(band_t b);
const char* band_freq(const char* band_name);

// psk_browser enums
enum { VIEWER_LABEL_OFF, VIEWER_LABEL_AF, VIEWER_LABEL_RF, VIEWER_LABEL_CH, VIEWER_LABEL_NTYPES };

#endif
