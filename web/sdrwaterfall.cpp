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

#include <iostream>
#include <cstdio>
#include <sndfile.h>
#include <ipp.h>
#include <ipps.h>
#include <ippac.h>

#include "IQFileAudioSource.h"

const int NSAMPLES = 8096;
const int NFFT = NSAMPLES;
const int ORDER = 13;
const int OUTPUT_SAMPLES = 1024;

void sample_delay()
{
	usleep(42666);
}

void fftshift(Ipp32f* b, int len)
{
        static Ipp32f* tmp = ippsMalloc_32f(len/2);
        ippsCopy_32f(b, tmp, len/2);
        ippsCopy_32f(b+len/2, b, len/2);
        ippsCopy_32f(tmp, b+len/2, len/2);
        
}

void downsample(Ipp32f* pwrspctr, int insize, int outsize)
{
	int binsize = insize/outsize;
	static Ipp32f* outmax = ippsMalloc_32f(OUTPUT_SAMPLES);
	for(int n=0; n<outsize;n++)
	{
		ippsMax_32f(pwrspctr+n*binsize, binsize, outmax+n);
	}
	ippsCopy_32f(outmax, pwrspctr, outsize);
}

int main()
{

	ippStaticInit();
	std::cout << "Content-type: sdr/waterfall"  << std::endl << std::endl<<std::flush;

	AudioSource* src = new IQFileAudioSource("SSB_IQ.wav", NSAMPLES);
	IppsFFTSpec_C_32f* fft_specs;
	ippsFFTInitAlloc_C_32f(&fft_specs, ORDER, IPP_FFT_DIV_BY_SQRTN, ippAlgHintNone);

	Ipp32f* buf = ippsMalloc_32f(2*NSAMPLES);
	Ipp32f* in_re = ippsMalloc_32f(NFFT);
	Ipp32f* in_im = ippsMalloc_32f(NFFT);
	Ipp32f* dst_im = ippsMalloc_32f(NFFT);
	Ipp32f* dst_re = ippsMalloc_32f(NFFT);
	Ipp32f* pwrspctr = ippsMalloc_32f(NFFT);
	Ipp8u* pwrspctr8u = ippsMalloc_8u(NFFT);
	int len;
	while (src->hasMore()) {
		src->getSamples(buf);
		Ipp32f* in[2];
		in[0] = in_re;
		in[1] = in_im;
		ippsDeinterleave_32f(buf, 2, NFFT, in);
		ippsFFTFwd_CToC_32f(in_re, in_im, dst_re, dst_im, fft_specs, NULL);
		fftshift(dst_re, NFFT);
		fftshift(dst_im, NFFT);
		ippsPowerSpectr_32f(dst_re, dst_im, pwrspctr, NFFT);
		downsample(pwrspctr, NFFT, OUTPUT_SAMPLES);
		ippsLn_32f_I(pwrspctr, OUTPUT_SAMPLES);
		ippsMulC_32f_I(4.34f, pwrspctr, OUTPUT_SAMPLES);
		ippsAddC_32f_I(85.0f, pwrspctr, OUTPUT_SAMPLES);
		ippsMulC_32f_I(4.0, pwrspctr, OUTPUT_SAMPLES);
		ippsConvert_32f8u_Sfs(pwrspctr, pwrspctr8u, OUTPUT_SAMPLES, ippRndNear, 0);

		std::cout.write((char*)pwrspctr8u, sizeof(Ipp8u)*OUTPUT_SAMPLES);
	}
}
