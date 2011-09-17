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

#include "session.h"

#include <string>
#include "md5wrapper.h"

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/HTTPStatusHeader.h"
#include "cgicc/HTTPCookie.h"

#include<stdexcept>

std::string generateSessionID()
{
	const time_t rawtime = time(NULL);
	std::string timestr(ctime(&rawtime));
	md5wrapper md5;
	std::string sessid = md5.getHashFromString(timestr);
	return sessid;
}

std::string getSessionID(cgicc::Cgicc& cgi)
{
        cgicc::CgiEnvironment env = cgi.getEnvironment();
        cgicc::const_cookie_iterator itr;
        bool found = false;
        std::string sessid;
        for(itr=env.getCookieList().begin();
            itr!=env.getCookieList().end();
            itr++)
        {
                if(itr->getName()=="sessid")
                {
                        found = true;
                        sessid = itr->getValue();
                        break;
                }
        }
	if(!found)
	{
		throw std::runtime_error("No sessid cookie found");
	}
	return sessid;

}
