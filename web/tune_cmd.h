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

#ifndef _TUNE_CMD_H_
#define _TUNE_CMD_H_

#include <ipp.h>

struct TuneCmdMsg
{
	static const int SET_FREQ=1;
	static const int SET_FILT=2;
	static const int SET_SIDEBAND=4;
	static const int SET_AGC_ATTACK=8;

	unsigned int cmd;
	Ipp32f lo;
	Ipp32f filt_low;
	Ipp32f filt_high;
	bool sideband;
	int agc_attack;
};

#endif
