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

#include "AUFileAudioSink.h"
#include "OGGFileAudioSink.h"
#include "IQFileAudioSource.h"
#include "IQSSBDemodulator.h"

#include <ipps.h>
#include <string>
#include <cstdlib>

#include <iostream>
#include <cstdio>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <errno.h>

#include <pthread.h>

mqd_t mq;

void* msgreader(void* arg)
{
	IQSSBDemodulator* demodulator = (IQSSBDemodulator*)arg;
	char* buf = (char*)malloc(32767);
	while(true)
	{
		ssize_t s = mq_receive(mq, buf, 32767, 0);
		if(s>0)
		{
			Ipp32f lo = atof(buf);
			demodulator->setLO(lo);
		}
	}
}

void setup_queue(IQSSBDemodulator* demodulator)
{
	mq=mq_open("/sdr", O_RDONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO, NULL);
	if(mq<0)
	{
		perror(NULL);
		exit(-1);
	}
	pthread_t thread;
	pthread_create(&thread, NULL, msgreader, demodulator);
}

void process(Ipp32f lo, const std::string& infile)
{
	IQSSBDemodulator* demodulator = new IQSSBDemodulator();
	setup_queue(demodulator);
	Ipp32f* iq_block = ippsMalloc_32f(4096);
	Ipp32f* demodulated_block = ippsMalloc_32f(2048);
	AudioSource* source = new IQFileAudioSource(infile, 2048, false); 
	AudioSink* sink = new AUFileAudioSink("outfile.au", 2048);
	demodulator->setLO(lo);

	while(source->hasMore())
	{
		source->getSamples(iq_block);
		demodulator->process_block(iq_block, demodulated_block);
		sink->playSamples(demodulated_block);
	}	
}


int main(int argc, char* argv[])
{
	char  buffer [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, buffer, sizeof (buffer)) ;
	std::cout << "sndfile version " << buffer << std::endl;
	ippStaticInit();


	Ipp32f lo = atoi(argv[2])/48000.0;
	std::string infile(argv[1]);

	process(lo, infile);
}
