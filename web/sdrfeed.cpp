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
//#include "OGGFileAudioSink.h"
#include "IQFileAudioSource.h"
#include "IQSSBDemodulator.h"

#include <ipps.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <cstdio>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <errno.h>
#include <time.h>

#include <pthread.h>

#include <signal.h>
#include <cstring>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPContentHeader.h"
#include "cgicc/HTTPStatusHeader.h"
#include "cgicc/HTTPCookie.h"

#include "session.h"
#include "tune_cmd.h"

mqd_t mq;
std::string sessid;
bool loop = true;

void sighandler(int num)
{
	mqd_t m = mq_close(mq);
	if(m<0)
	{
		std::cerr << "sdrfeed: mq_close error: " << strerror(errno) << std::endl;
	}
	std::string q_name("/sdr");
	q_name.append(sessid);
	m = mq_unlink(q_name.c_str());
	if(m<0)
	{
		std::cerr << "sdrfeed: mq_unlink error for " << sessid << " : " << strerror(errno) << std::endl;
	}
}

void parse_msg(const std::string& msg)
{
	size_t found = msg.find('=');
	if(found!=std::string::npos)
	{
		;
	}
	std::string var=msg.substr(0, found);
	std::string val=msg.substr(found+1,std::string::npos);

}

void* msgreader(void* arg)
{
        IQSSBDemodulator* demodulator = (IQSSBDemodulator*)arg;
        char* buf = (char*)malloc(32767);
        while(true)
        {
                ssize_t s = mq_receive(mq, buf, 32767, 0);
                if(s>0)
                {
			TuneCmdMsg* msg = (TuneCmdMsg*)buf;
			if(msg->cmd & TuneCmdMsg::SET_FREQ)
			{
				demodulator->setLO(msg->lo);
			}
			if(msg->cmd & TuneCmdMsg::SET_FILT)
			{
				demodulator->setFilter(msg->filt_low, msg->filt_high);
			}
			if(msg->cmd & TuneCmdMsg::SET_SIDEBAND)
			{
				if(msg->sideband == false)
					demodulator->setSideband(IQSSBDemodulator::LSB);
				else
					demodulator->setSideband(IQSSBDemodulator::USB);
			}
                }
        }
}

void setup_queue(IQSSBDemodulator* demodulator)
{
	std::string q_name("/sdr");
	q_name.append(sessid);
        mq=mq_open(q_name.c_str(), O_RDONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO, NULL);
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

	Ipp32f* iq_block = ippsMalloc_32f(4096);
	Ipp32f* demodulated_block = ippsMalloc_32f(2048);
	AudioSource* source = new IQFileAudioSource(infile, 2048, loop); 
	AudioSink* sink = new AUFileAudioSink("stdout", 2048);
	IQSSBDemodulator* demodulator = new IQSSBDemodulator();
	setup_queue(demodulator);
	demodulator->setLO(lo);

	while(source->hasMore())
	{
		source->getSamples(iq_block);
		demodulator->process_block(iq_block, demodulated_block);
		sink->playSamples(demodulated_block);
	}	
}

void signal_init()
{
	struct sigaction sa;
	sa.sa_handler = sighandler;
	sigaction(SIGTERM, &sa, NULL);
}

int main(int argc, char* argv[])
{
	try {
		cgicc::Cgicc cgi;
                sessid = getSessionID(cgi);
//		std::string noloop = cgi["NOLOOP"]->getValue();
//		std::string noloop = cgi("NOLOOP");
//		if(noloop == "true")
//		{
//			loop = false;
//		}
	}
	catch(std::runtime_error& e)
	{
//		std::cout << cgicc::HTTPStatusHeader(400, "SessionID cookie missing");
//		return EXIT_SUCCESS;
//		sessid = "x";
	}

	signal_init();
	ippStaticInit();

	cgicc::HTTPCookie kaka("sessid",sessid);
	kaka.setPath("/");
        std::cout << cgicc::HTTPContentHeader("audio/basic").setCookie(kaka)<< std::flush;

	Ipp32f lo = 0;
	process(lo, "SSB_IQ.wav");

	return EXIT_SUCCESS;
}
