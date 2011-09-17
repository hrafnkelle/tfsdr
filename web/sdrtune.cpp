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

#include <cstdlib>
#include <cstdio>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <errno.h>
#include <cstring>


#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/HTTPStatusHeader.h"
#include "cgicc/HTTPCookie.h"
#include "session.h"
#include "tune_cmd.h"

#include <stdexcept>

int main(int argc, char* argv[])
{
	std::string sessid;
	cgicc::Cgicc cgi;
	try {
		sessid = getSessionID(cgi);
	}
	catch (std::runtime_error& e)
	{
		std::cout << cgicc::HTTPStatusHeader(400, "No session ID cookie found") << std::flush;
		return EXIT_SUCCESS;
	}
	std::string q_name("/sdr");
	q_name.append(sessid);
	mqd_t mq=mq_open(q_name.c_str(), O_WRONLY);
	if(mq<0)
	{
		std::cout << cgicc::HTTPStatusHeader(400, strerror(errno))<< std::flush;
		return EXIT_SUCCESS;
	}

	TuneCmdMsg* msg = new TuneCmdMsg;
	std::string lo_str = cgi["LO"]->getValue();
	if(lo_str != "")
	{
		int lo_freq = atoi(lo_str.c_str());
		msg->cmd |= TuneCmdMsg::SET_FREQ;
		msg->lo = lo_freq/48000.0;
	}
	std::string fl_str = cgi["FL"]->getValue();
	std::string fh_str = cgi["FH"]->getValue();
	if((fl_str!="")&&(fh_str!=""))
	{
		msg->cmd |= TuneCmdMsg::SET_FILT;
		int l = atoi(fl_str.c_str());
		int h = atoi(fh_str.c_str());
		msg->filt_low = l/48000.0;
		msg->filt_high = h/48000.0;
	}
	std::string sb_str = cgi["SB"]->getValue();
	if(sb_str!="")
	{
		msg->cmd |= TuneCmdMsg::SET_SIDEBAND;
		if(sb_str=="LSB")
		{
			msg->sideband = false;
		}
		else if(sb_str=="USB")
		{
			msg->sideband = true;
		}
	}

	mq_send(mq, (const char*)msg, sizeof(TuneCmdMsg), 0);
	std::cout << cgicc::HTTPStatusHeader(200, "Mange Tak!")<<std::flush;
	return EXIT_SUCCESS;
}
