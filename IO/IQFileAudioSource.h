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

#ifndef _IQFILE_AUDIO_SOURCE_H_
#define _IQFILE_AUDIO_SOURCE_H_

#include <sndfile.h>
#include <string>

#include "AudioSource.h"

class IQFileAudioSource : public AudioSource
{
public:
	IQFileAudioSource(const std::string& filename, int samples_in_block, bool loop=true);
	virtual void getSamples(Ipp32f* samples_buf);
	virtual bool hasMore();
private:
	SNDFILE* _f;	
	SF_INFO _wav_info;
	const int _nsamples;
	bool _done;
	bool _loop;

	unsigned int _block_delay;
};
#endif
