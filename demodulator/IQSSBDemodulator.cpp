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
#include "IQSSBDemodulator.h"

#include <ipps.h>
#include <iostream>

IQSSBDemodulator::IQSSBDemodulator()
{
	_sideband = USB;

	ippsFFTInitAlloc_C_32f(&_fft_specs, ORDER, IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone);
	int fft_bufsize;
	ippsFFTGetBufSize_C_32f(_fft_specs, &fft_bufsize);
	_fft_buf = ippsMalloc_8u(fft_bufsize);
	// allocate buffers
	_in_re = ippsMalloc_32f(NFFT);
	_in_im = ippsMalloc_32f(NFFT);
	_in_p = ippsMalloc_32f(NFFT);
	_in_m = ippsMalloc_32f(NFFT);
	_residual_re = ippsMalloc_32f(NFFT);
	_residual_im = ippsMalloc_32f(NFFT);

	// prepare for ippsDeinterleave
	_iq[0] = _in_re;
	_iq[1] = _in_im;
	
	// design initial filter
	_taps_len = BLKSIZE;
	_fir_taps_re = ippsMalloc_32f(NFFT);
	_fir_taps_im= ippsMalloc_32f(NFFT);
	_fir_taps_m  = ippsMalloc_32f(NFFT);  // magnitude
	_fir_taps_p  = ippsMalloc_32f(NFFT);  // phase
	_lowFreq = 200.0/48000.0;
	_highFreq = 3000.0/48000.0;
	this->setFilter(_lowFreq, _highFreq);

	_residual_length = NFFT - _taps_len -1;
}

void IQSSBDemodulator::fftshift(Ipp32f* b, int len)
{
	static Ipp32f* tmp = ippsMalloc_32f(len/2);
	ippsCopy_32f(b, tmp, len/2);
	ippsCopy_32f(b+len/2, b, len/2);
	ippsCopy_32f(tmp, b+len/2, len/2);
	
}


void IQSSBDemodulator::circshift(Ipp32f* b, int len, int lo)
{
	fftshift(b, len);
	static Ipp32f* tmp = ippsMalloc_32f(len);
	if(lo>=0)
	{
		ippsCopy_32f(b, tmp, lo);
		ippsMove_32f(b+lo, b, len-lo); 
		ippsCopy_32f(tmp, b+len-lo, lo);
	}
	else
	{
		int abs_lo = -lo;
		ippsCopy_32f(b+len-abs_lo, tmp, abs_lo);
		ippsMove_32f(b, b+abs_lo, len-abs_lo);
		ippsCopy_32f(tmp, b, abs_lo);
	}

	fftshift(b, len);
}

void IQSSBDemodulator::process_block(Ipp32f* iq_block, Ipp32f* out_block)
{
	static bool flip=false;
	// Deinterleave to real and imaginary (I and Q) buffers
	ippsDeinterleave_32f(iq_block, 2, BLKSIZE, _iq);
	ippsZero_32f(_in_re+BLKSIZE, NFFT-BLKSIZE);
	ippsZero_32f(_in_im+BLKSIZE, NFFT-BLKSIZE);
	// _in_re now contains the real/I part and 
	// _in_im now contains the imaginary/Q part
	
	ippsFFTFwd_CToC_32f_I(_in_re, _in_im,
	                       _fft_specs, _fft_buf);
	ippsCartToPolar_32f(_in_re, _in_im,
	                    _in_m, _in_p,
	                    NFFT);

	// layout of frequency bins is
	// NFFT/2 to NFFT-1 and then continues from 0 to NFFT/2-1

	// shift desired part to 0Hz
	int lo = _lo*NFFT;
	circshift(_in_m, NFFT, lo);	
	circshift(_in_p, NFFT, lo);	
	// zero out undesired sideband
	if(_sideband == USB)
	{
		// zero out LSB, that is NFFT/2 to NFFT-1
		ippsZero_32f(_in_m+NFFT/2, NFFT/2);
		ippsZero_32f(_in_p+NFFT/2, NFFT/2);
	}
	else // _sideband must be LSB
	{
		// zero out USB, that is 0 to NFFT/2-1
		ippsZero_32f(_in_m, NFFT/2);
		ippsZero_32f(_in_p, NFFT/2);
	}
	// filter the passband
	ippsMul_32f_I(_fir_taps_m, _in_m, NFFT);
	ippsAdd_32f_I(_fir_taps_p, _in_p, NFFT);
	// return to time domain
	ippsPolarToCart_32f(_in_m, _in_p,
	                    _in_re, _in_im,
	                    NFFT);
	ippsFFTInv_CToC_32f_I(_in_re, _in_im,
	                    _fft_specs, _fft_buf);
	// do overlap/add
	//
	// 1) add the residual from last round
	ippsAdd_32f_I(_residual_re, _in_re, _residual_length);
	ippsAdd_32f_I(_residual_im, _in_im, _residual_length);
	// 2) Store the new residual
	if(flip)
	{
		ippsMulC_32f_I(-1.0, _in_re, NFFT);
		ippsMulC_32f_I(-1.0, _in_im, NFFT);
		flip=!flip;
	}
	ippsCopy_32f(_in_re+BLKSIZE, _residual_re, _residual_length);
	ippsCopy_32f(_in_im+BLKSIZE, _residual_im, _residual_length);

	// agc
	agc(_in_re, BLKSIZE);
	
	// deliver the result
	ippsCopy_32f(_in_re, out_block, BLKSIZE);
}

void IQSSBDemodulator::setFilter(Ipp64f lowFreq, Ipp64f highFreq)
{
	_lowFreq = lowFreq;
	_highFreq = highFreq;
	ippsZero_32f(_fir_taps_re, NFFT);
	ippsZero_32f(_fir_taps_im, NFFT);
	Ipp64f* fir_tmp = ippsMalloc_64f(_taps_len);
	ippsFIRGenBandpass_64f(_lowFreq, _highFreq, fir_tmp, _taps_len,
	                       ippWinHamming, ippTrue);
	ippsConvert_64f32f(fir_tmp, _fir_taps_re, _taps_len);
	ippsFree(fir_tmp);

	ippsFFTFwd_CToC_32f_I(_fir_taps_re, _fir_taps_im,
	                       _fft_specs, _fft_buf);
	ippsCartToPolar_32f(_fir_taps_re, _fir_taps_im,
	                    _fir_taps_m, _fir_taps_p,
	                    NFFT);
}

void IQSSBDemodulator::setLO(Ipp32f lo)
{
	_lo = lo;
}

void IQSSBDemodulator::setSideband(sideband_t sb)
{
	_sideband = sb;
}

void IQSSBDemodulator::agc(Ipp32f* b, int len)
{
	const Ipp32f MAX_GAIN = 1000.0f;
	const int AGC_HANG = 10;
	const int AGC_ATTACK_COUNT = 48;
	static Ipp32f prev_gain = 0;
	static int agcloop = 0;
	static Ipp32f* gain_mult = ippsMalloc_32f(BLKSIZE);
	static Ipp32f* gain_factor = ippsMalloc_32f(AGC_HANG);

	agcloop = (agcloop+1)%AGC_HANG;
	
	Ipp32f Vpk;
	ippsMax_32f(b, len, &Vpk);
	Ipp32f gain = MAX_GAIN;
	if (Vpk>(100*IPP_MINABS_32F))
	{
		gain_factor[agcloop] = 0.5/Vpk;
		ippsMin_32f(gain_factor, AGC_HANG, &gain);
	}
	if (gain>MAX_GAIN)
	{
		gain = MAX_GAIN;
	}
	ippsSet_32f(gain, gain_mult, len);
	Ipp32f gain_step = (gain-prev_gain)/(AGC_ATTACK_COUNT+0.0);
	for(int n=0; n<AGC_ATTACK_COUNT; n++)
	{
		gain_mult[n] = prev_gain+n*gain_step;
	}
	prev_gain = gain;
	ippsMul_32f_I(gain_mult, b, len);
}
