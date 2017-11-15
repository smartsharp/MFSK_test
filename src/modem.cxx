// ----------------------------------------------------------------------------
// modem.cxx - modem class - base for all modems
//
// Copyright (C) 2006-2010
//		Dave Freese, W1HKJ
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

#include <string>
#include <cstdlib>

#include "misc.h"
#include "filters.h"
#include "modem.h"
#include "util.h"

using namespace std;

modem *null_modem = 0;

modem *mfsk8_modem = 0;
modem *mfsk16_modem = 0;
modem *mfsk32_modem = 0;
// experimental modes
modem *mfsk4_modem = 0;
modem *mfsk11_modem = 0;
modem *mfsk22_modem = 0;
modem *mfsk31_modem = 0;
modem *mfsk64_modem = 0;
modem *mfsk128_modem = 0;
modem *mfsk64l_modem = 0;
modem *mfsk128l_modem = 0;

modem *psk31_modem = 0;
modem *psk63_modem = 0;
modem *psk63f_modem = 0;
modem *psk125_modem = 0;
modem *psk250_modem = 0;
modem *psk500_modem = 0;
modem *psk1000_modem = 0;

modem *qpsk31_modem = 0;
modem *qpsk63_modem = 0;
modem *qpsk125_modem = 0;
modem *qpsk250_modem = 0;
modem *qpsk500_modem = 0;

modem *_8psk125_modem = 0;
modem *_8psk250_modem = 0;
modem *_8psk500_modem = 0;
modem *_8psk1000_modem = 0;
modem *_8psk1200_modem = 0;
modem *_8psk1333_modem = 0;

modem *_8psk125fl_modem = 0;
modem *_8psk125f_modem = 0;
modem *_8psk250fl_modem = 0;
modem *_8psk250f_modem = 0;
modem *_8psk500f_modem = 0;
modem *_8psk1000f_modem = 0;
modem *_8psk1200f_modem = 0;
modem *_8psk1333f_modem = 0;

modem *psk125r_modem = 0;
modem *psk250r_modem = 0;
modem *psk500r_modem = 0;
modem *psk1000r_modem = 0;

modem *psk800_c2_modem = 0;
modem *psk800r_c2_modem = 0;
modem *psk1000_c2_modem = 0;
modem *psk1000r_c2_modem = 0;

modem *psk63r_c4_modem = 0;
modem *psk63r_c5_modem = 0;
modem *psk63r_c10_modem = 0;
modem *psk63r_c20_modem = 0;
modem *psk63r_c32_modem = 0;

modem *psk125r_c4_modem = 0;
modem *psk125r_c5_modem = 0;
modem *psk125r_c10_modem = 0;
modem *psk125_c12_modem = 0;
modem *psk125r_c12_modem = 0;
modem *psk125r_c16_modem = 0;

modem *psk250r_c2_modem = 0;
modem *psk250r_c3_modem = 0;
modem *psk250r_c5_modem = 0;
modem *psk250_c6_modem = 0;
modem *psk250r_c6_modem = 0;
modem *psk250r_c7_modem = 0;

modem *psk500_c2_modem = 0;
modem *psk500_c4_modem = 0;

modem *psk500r_c2_modem = 0;
modem *psk500r_c3_modem = 0;
modem *psk500r_c4_modem = 0;

modem *olivia_modem = 0;
modem *olivia_4_250_modem = 0;
modem *olivia_8_250_modem = 0;
modem *olivia_4_500_modem = 0;
modem *olivia_8_500_modem = 0;
modem *olivia_16_500_modem = 0;
modem *olivia_8_1000_modem = 0;
modem *olivia_16_1000_modem = 0;
modem *olivia_32_1000_modem = 0;
modem *olivia_64_2000_modem = 0;


double modem::frequency = 1000;
double modem::tx_frequency = 1000;
bool   modem::freqlock = false;

// For xml socket command
unsigned long modem::tx_sample_count = 0;
unsigned int  modem::tx_sample_rate  = 0;

modem::modem()
{
	scptr = 0;
	freqlock = false;
	frequency = tx_frequency = 1000;

	sigsearch = 0;
	reverse = false;
	cap = CAP_RX | CAP_TX;
	bandwidth = 0.0;
	afconoff = true;
	sqlonoff = true;
	pskpilot = false;
	psk8DCDShortFlag = false;
	pilot_power = -30.0;
}

// modem types CW and RTTY do not use the base init()
void modem::init()
{
	stopflag = false;
	reverse = false;
}

void modem::set_freq(double freq)
{
	frequency = CLAMP(
		freq,
		0/*progdefaults.LowFreqCutoff*/ + bandwidth / 2,
		3000/*progdefaults.HighFreqCutoff*/ - bandwidth / 2);
	if (freqlock == false)
		tx_frequency = frequency;
	//REQ(put_freq, frequency);
}

void modem::set_freqlock(bool on)
{
	freqlock = on;
	set_freq(frequency);
}


double modem::get_txfreq(void) const
{
	if (unlikely(!(cap & CAP_TX)))
		return 0;
	//else if (mailserver && progdefaults.PSKmailSweetSpot)
	//	return progdefaults.PSKsweetspot;
	//if (get_mode() == MODE_FSQ) return 1500;
	return tx_frequency;
}

double modem::get_txfreq_woffset(void) const
{
	//if (mailserver && progdefaults.PSKmailSweetSpot)
	//	return (progdefaults.PSKsweetspot - progdefaults.TxOffset);
	//if (get_mode() == MODE_FSQ) return (1500 - progdefaults.TxOffset);
	return (tx_frequency - 0/*progdefaults.TxOffset*/);
}

void modem::set_bandwidth(double bw)
{
	bandwidth = bw;
	//put_Bandwidth((int)bandwidth);
}

void modem::set_reverse(bool on)
{
	//if (likely(wf))
	//	reverse = on ^ (!wf->USB());
	//else
		reverse = false;
}

void modem::set_metric(double m)
{
	metric = m;
}

extern void callback_set_metric(double metric);

void modem::display_metric(double m)
{
	set_metric(m);
	//if (!progStatus.kpsql_enabled)
	//	REQ(callback_set_metric, m);
}


void modem::set_samplerate(int smprate)
{
	samplerate = smprate;
}


bool disable_modem = false;

void modem::ModulateXmtr(double *buffer, int len)
{
	//if (unlikely(!TXscard)) return;
	if (disable_modem) return;

	tx_sample_count += len;

	/*if (progdefaults.PTTrightchannel) {
		for (int i = 0; i < len; i++)
			PTTchannel[i] = PTTnco();
		ModulateStereo( buffer, PTTchannel, len, false);
		return;
	}*/

	//if (withnoise) add_noise(buffer, len);

	/*if (progdefaults.viewXmtSignal &&
		!(PERFORM_CPS_TEST || active_modem->XMLRPC_CPS_TEST))
		trx_xmit_wfall_queue(samplerate, buffer, (size_t)len);*/

	double mult = pow(10, -3.0/*progdefaults.txlevel*/ / 20.0);
	if (mult > 0.99) mult = 0.99;
	for (int i = 0; i < len; i++) {
		buffer[i] *= mult;
		if (buffer[i] < -0.99) buffer[i] = -0.99;
		if (buffer[i] >  0.99) buffer[i] = 0.99;
	}
	on_tx_write(buffer, len);
	/*
	try {
		unsigned n = 4;
		while (TXscard->Write(buffer, len) == 0 && --n);
		if (n == 0)
			throw SndException(-99, "Sound write failed");
	}
	catch (const SndException& e) {
		if(e.error() < 0) {
 			LOG_ERROR("%s", e.what());
 			throw;
		}
		return;
	}*/
}
int modem::getbuflen(){
	return 512;
}
