/*
 * Copyright 2011 Hrafnkell Eiriksson he@klaki.net TF3HR
 * 
 * This file is part of TFSDR.
 *
 * TFSDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TFSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TFSDR.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _IQ_SSB_DEMODULATOR_H_
#define _IQ_SSB_DEMODULATOR_H_

#include <ipp.h>

class IQSSBDemodulator
{
public:
	typedef enum {USB, LSB} sideband_t;
	static const int NFFT = 4096;
	static const int BLKSIZE = NFFT/2;

	IQSSBDemodulator();
	void process_block(Ipp32f* iq_block, Ipp32f* out_block);

	void setFilter(int low_cutoff, int high_cutoff);
	void setLO(Ipp32f lo);
	void setSideband(sideband_t s);
	void setFilter(Ipp64f lowFreq, Ipp64f highFreq);
private:
	void fftshift(Ipp32f* f, int len);
	void circshift(Ipp32f* f, int len, int lo);

	void agc(Ipp32f* b, int len);

	static const int ORDER = 12;
	IppsFFTSpec_C_32f* _fft_specs;
	Ipp8u* _fft_buf;
	Ipp32f* _in_re;
	Ipp32f* _in_im;
	Ipp32f* _in_m;
	Ipp32f* _in_p;
	Ipp32f* _residual_re;
	Ipp32f* _residual_im;
	Ipp32f* _iq[2]; 

	sideband_t  _sideband;
	Ipp32f _lo;

	// FIR bandpass filter stuff
	Ipp64f _lowFreq;
	Ipp64f _highFreq;
	Ipp32f* _fir_taps_re;
	Ipp32f* _fir_taps_im;
	Ipp32f* _fir_taps_m;
	Ipp32f* _fir_taps_p;
	int _taps_len;

	int _residual_length;
};

#endif
