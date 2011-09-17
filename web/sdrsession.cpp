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

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/CgiEnvironment.h"
#include "cgicc/HTTPContentHeader.h"
#include "cgicc/HTTPStatusHeader.h"
#include "cgicc/HTTPRedirectHeader.h"
#include "cgicc/HTTPCookie.h"

#include "session.h"

int main(int argc, char* argv[])
{
	cgicc::Cgicc cgi;
	std::string sessid = generateSessionID();
	cgicc::HTTPCookie kaka("sessid", sessid);
	kaka.setPath("/");
//	std::cout << cgicc::HTTPStatusHeader(200,"OK").setCookie(kaka);
	std::string query_string = cgi.getEnvironment().getQueryString();
	std::string sdrfeed_name = "sdrfeed?";
	sdrfeed_name.append(query_string);
	std::cout << cgicc::HTTPRedirectHeader(sdrfeed_name).setCookie(kaka);

	return EXIT_SUCCESS;
}