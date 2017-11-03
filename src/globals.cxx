// ----------------------------------------------------------------------------
// globals.cxx  --  constants, variables, arrays & functions that need to be
//                  outside of any thread
//
// Copyright (C) 2006-2009
//		Dave Freese, W1HKJ
// Copyright (C) 2007-2009
//		Stelios Bounanos, M0GLD
//
// This file is part of fldigi.
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

//#include <config.h>
#include <iosfwd>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <string.h>

#include "globals.h"
#include "modem.h"
#include "util.h"

using namespace std;

// ************ Elements are in enum trx_mode order. ************
// N.B. it is not valid to use an _("NLS") string in this table!!
// ... doing so will break the Fl_menu_item table 'menu_'.  -Kamal

// Last value (true/false) determines if it's used with the KISS interface.
// It must have 8 bit support. Current selection based on the modems used in FLAMP.

const struct mode_info_t mode_info[NUM_MODES] = {
	{ MODE_NULL, &null_modem, "NULL", "NULL", "", "NULL", "", DISABLED_IO },

	{ MODE_MFSK8, &mfsk8_modem, "MFSK8", "MFSK-8", "MFSK8", "MFSK8", "MK 8", DISABLED_IO  },
	{ MODE_MFSK16, &mfsk16_modem, "MFSK16", "MFSK-16", "MFSK16", "MFSK16", "MK16", ARQ_IO | KISS_IO },
	{ MODE_MFSK32, &mfsk32_modem, "MFSK32", "MFSK-32", "MFSK32", "MFSK32", "MK32", ARQ_IO | KISS_IO  },

	{ MODE_MFSK4, &mfsk4_modem, "MFSK4", "MFSK-4", "MFSK4", "MFSK4", "MK 4", DISABLED_IO  },
	{ MODE_MFSK11, &mfsk11_modem, "MFSK11", "MFSK-11", "MFSK11", "MFSK11", "MK11", DISABLED_IO  },
	{ MODE_MFSK22, &mfsk22_modem, "MFSK22", "MFSK-22", "MFSK22", "MFSK22", "MK22", DISABLED_IO  },
	{ MODE_MFSK31, &mfsk31_modem, "MFSK31", "MFSK-31", "MFSK31", "MFSK31", "MK31", ARQ_IO | KISS_IO },
	{ MODE_MFSK64, &mfsk64_modem, "MFSK64", "MFSK-64", "MFSK64", "MFSK64", "MK64", ARQ_IO | KISS_IO   },
	{ MODE_MFSK128, &mfsk128_modem, "MFSK128", "MFSK-128", "MFSK128", "MFSK128", "MK128", ARQ_IO | KISS_IO   },
	{ MODE_MFSK64L, &mfsk64l_modem, "MFSK64L", "MFSK-64L", "MFSK64L", "MFSK64L", "MK64L", ARQ_IO | KISS_IO   },
	{ MODE_MFSK128L, &mfsk128l_modem, "MFSK128L", "MFSK-128L", "MFSK128L", "MFSK128L", "MK128L", ARQ_IO | KISS_IO  },

};

std::ostream& operator<<(std::ostream& s, const qrg_mode_t& m)
{
	return s << m.rfcarrier << ' '
			 << m.rmode << ' '
			 << m.carrier << ' '
			 << mode_info[m.mode].sname << ' '
			 << m.usage;
}

std::istream& operator>>(std::istream& s, qrg_mode_t& m)
{
	string sMode;
	char temp[255];
	int mnbr;
	s >> m.rfcarrier >> m.rmode >> m.carrier >> sMode;

	s.getline(temp, 255);
	m.usage = temp;
	while (m.usage[0] == ' ') m.usage.erase(0,1);

// handle case for reading older type of specification string
	if (sscanf(sMode.c_str(), "%d", &mnbr)) {
		m.mode = mnbr;
		return s;
	}
	m.mode = MODE_MFSK16;
	for (mnbr = MODE_MFSK8; mnbr < NUM_MODES; mnbr++)
		if (sMode == mode_info[mnbr].sname) {
			m.mode = mnbr;
			break;
		}
	return s;
}

std::string qrg_mode_t::str(void)
{
	ostringstream s;
	s << setiosflags(ios::fixed)
	  << setprecision(3) << rfcarrier/1000.0 << '|'
	  << rmode << '|'
	  << (mode < NUM_MODES ? mode_info[mode].sname : "NONE") << '|'
//	  << carrier;
	  << carrier << '|'
	  << usage;
	return s.str();
}

band_t band(long long freq_hz)
{
	switch (freq_hz / 1000000LL) {
		case 0: case 1: return BAND_160M;
		case 3: return BAND_80M;
		case 4: return BAND_75M;
		case 5: return BAND_60M;
		case 7: return BAND_40M;
		case 10: return BAND_30M;
		case 14: return BAND_20M;
		case 18: return BAND_17M;
		case 21: return BAND_15M;
		case 24: return BAND_12M;
		case 28 ... 29: return BAND_10M;
		case 50 ... 54: return BAND_6M;
		case 70 ... 71: return BAND_4M;
		case 144 ... 148: return BAND_2M;
		case 222 ... 225: return BAND_125CM;
		case 420 ... 450: return BAND_70CM;
		case 902 ... 928: return BAND_33CM;
		case 1240 ... 1325: return BAND_23CM;
		case 2300 ... 2450: return BAND_13CM;
		case 3300 ... 3500: return BAND_9CM;
		case 5650 ... 5925: return BAND_6CM;
		case 10000 ... 10500: return BAND_3CM;
		case 24000 ... 24250: return BAND_125MM;
		case 47000 ... 47200: return BAND_6MM;
		case 75500 ... 81000: return BAND_4MM;
		case 119980 ... 120020: return BAND_2P5MM;
		case 142000 ... 149000: return BAND_2MM;
		case 241000 ... 250000: return BAND_1MM;
	}

	return BAND_OTHER;
}

band_t band(const char* freq_mhz)
{
	errno = 0;
	double d = strtod(freq_mhz, NULL);
	if (d != 0.0 && errno == 0)
		return band((long long)(d * 1e6));
	else
		return BAND_OTHER;
}

struct band_freq_t {
	const char* band;
	const char* freq;
};

static struct band_freq_t band_names[NUM_BANDS] = {
	{ "160m", "1.8" },
	{ "80m", "3.5" },
	{ "75m", "4.0" },
	{ "60m", "5.3" },
	{ "40m", "7.0" },
	{ "30m", "10.0" },
	{ "20m", "14.0" },
	{ "17m", "18.0" },
	{ "15m", "21.0" },
	{ "12m", "24.0" },
	{ "10m", "28.0" },
	{ "6m", "50.0" },
	{ "4m", "70.0" },
	{ "2m", "144.0" },
	{ "1.25m", "222.0" },
	{ "70cm", "420.0" },
	{ "33cm", "902.0" },
	{ "23cm", "1240.0" },
	{ "13cm", "2300.0" },
	{ "9cm", "3300.0" },
	{ "6cm", "5650.0" },
	{ "3cm", "10000.0" },
	{ "1.25cm", "24000.0" },
	{ "6mm", "47000.0" },
	{ "4mm", "75500.0" },
	{ "2.5mm", "119980.0" },
	{ "2mm", "142000.0" },
	{ "1mm", "241000.0" },
	{ "other", "" }
};

const char* band_name(band_t b)
{
	return band_names[CLAMP(b, 0, NUM_BANDS-1)].band;
}

const char* band_name(const char* freq_mhz)
{
	return band_name(band(freq_mhz));
}

const char* band_freq(band_t b)
{
	return band_names[CLAMP(b, 0, NUM_BANDS-1)].freq;
}

const char* band_freq(const char* band_name)
{
	for (size_t i = 0; i < BAND_OTHER; i++)
		if (!strcmp(band_names[i].band, band_name))
			return band_names[i].freq;

	return "";
}
