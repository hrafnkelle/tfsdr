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

#ifndef _AUFILEAUDIOSINK_H_
#define _AUFILEAUDIOSINK_H_

#include "AudioSink.h"
#include <string>
#include <sndfile.h>

class AUFileAudioSink: public AudioSink
{
public:
	AUFileAudioSink(const std::string& filename, int out_count);
	virtual void playSamples(Ipp32f* audio_buf);
private:
	int _count;
	SNDFILE* _f;
	SF_INFO _sfinfo;

};

#endif
