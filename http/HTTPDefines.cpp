/*
 * HTTPDefines.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */


#include "HTTPDefines.h"

#include <iostream>

std::string HTTPContentType = "Content-type";
std::string HTTPHost = "Host";
std::string HTTPContentLength = "Content-length";
std::string HTTPUserAgent = "User-Agent";
std::string HTTPProtocolPrefix = "http://";


std::string
HostFromConnectionString(std::string string)
{
	std::cout << "HostFromConnectionString: " << string << std::endl;
	// TODO: Remove port if specified
	size_t prefixPos = string.find(HTTPProtocolPrefix);
	if (prefixPos == std::string::npos)
		return string;

	size_t slashPos = string.find('/', HTTPProtocolPrefix.length());
	return string.substr(HTTPProtocolPrefix.length(), slashPos);
}
