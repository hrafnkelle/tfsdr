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

#include "IQFileAudioSource.h"

#include <unistd.h>

IQFileAudioSource::IQFileAudioSource(const std::string& filename, int samples_in_block, bool loop) : AudioSource(), _nsamples(samples_in_block), _loop(loop)
{
	_wav_info.format = 0;
	_done = false;
	_f = sf_open(filename.c_str(), SFM_READ, &_wav_info);
	_block_delay = 0.95*(1.0/(_wav_info.samplerate/_nsamples+0.0))*1000*1000;
}

void IQFileAudioSource::getSamples(Ipp32f* samples_buf)
{
	int len = sf_read_float(_f, samples_buf, 2*_nsamples);
	if(len == 0)
	{
		sf_seek(_f, 0, SEEK_SET);
		len = sf_read_float(_f, samples_buf, _nsamples);
		if(!_loop)	
			_done = true;
	}
	usleep(_block_delay);
}

bool IQFileAudioSource::hasMore()
{
	return !_done;
}
