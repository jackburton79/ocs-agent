/*
 * HTTPDefines.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */


#include "HTTPDefines.h"
#include "URL.h"

#include <cstdlib>
#include <iostream>

std::string HTTPHost = "Host";
std::string HTTPContentType = "Content-Type";
std::string HTTPContentLength = "Content-Length";
std::string HTTPUserAgent = "User-Agent";
std::string HTTPProtocolPrefix = "http://";


int
GetHostAndPortFromString(const std::string& string, std::string& host,
	int& port)
{
	try {
		URL url(string.c_str());
		host = url.Host();
		port = url.Port();
	} catch (...) {
		return -1;
	}
	return 0;
}
