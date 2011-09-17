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

#include "OGGFileAudioSink.h"
#include<iostream>

OGGFileAudioSink::OGGFileAudioSink(const std::string& filename, int out_count) : _count(out_count)
{
	_sfinfo.samplerate = 8000;
	_sfinfo.channels = 1;
//	_sfinfo.format = SF_FORMAT_AU | SF_FORMAT_FLOAT;
	_sfinfo.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS;
//	_sfinfo.frames = 0x7FFFFFFF;
	int valid_format = sf_format_check(&_sfinfo);
	if(!valid_format)
		std::cerr << "invalid format" << std::endl;
	if(filename == "stdout")
	{
	 	_f = sf_open_fd(1, SFM_WRITE, &_sfinfo, false);

	}
	else
	{
	 	_f = sf_open(filename.c_str(), SFM_WRITE, &_sfinfo);
	}
	if(!_f)
	{
		std::cerr << "sf_open failed" << std::cerr;
	}
}

void OGGFileAudioSink::playSamples(Ipp32f* samples)
{
	static int phase = 0;
	static Ipp32f* downsamples = ippsMalloc_32f(_count);
	int downcount;
	ippsSampleDown_32f(samples, _count, downsamples, &downcount, 6, &phase);
	sf_count_t relcnt = sf_write_float(_f, downsamples, downcount);
}
