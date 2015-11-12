/*
 * HTTPDefines.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */


#include "HTTPDefines.h"

#include <iostream>

std::string HTTPHost = "Host";
std::string HTTPContentType = "Content-Type";
std::string HTTPContentLength = "Content-Length";
std::string HTTPUserAgent = "User-Agent";
std::string HTTPProtocolPrefix = "http://";


std::string
HostFromConnectionString(std::string string)
{
	// TODO: Remove port if specified
	size_t prefixPos = string.find(HTTPProtocolPrefix);
	if (prefixPos == std::string::npos)
		return string;

	size_t slashPos = string.find('/', HTTPProtocolPrefix.length());
	std::string result = string.substr(HTTPProtocolPrefix.length(),
			slashPos - HTTPProtocolPrefix.length());
	return result;
}
