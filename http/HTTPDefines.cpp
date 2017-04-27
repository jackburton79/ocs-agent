/*
 * HTTPDefines.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */


#include "HTTPDefines.h"

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
		// TODO: Remove port if specified
		std::string result = string;
		size_t prefixPos = string.find(HTTPProtocolPrefix);	
		if (prefixPos != std::string::npos) {
			// Remove protocol part (HTTP://)
			size_t slashPos = string.find('/', HTTPProtocolPrefix.length());
			result = string.substr(HTTPProtocolPrefix.length(),
					slashPos - HTTPProtocolPrefix.length());		
		}
		size_t portPos = result.find(":");
		if (portPos != std::string::npos) {
			host = result.substr(0, portPos);
			port = ::strtol(result.substr(portPos + 1, result.length()).c_str(),
				NULL, 10);
		} else {
			host = result;
			port = 80;
		}		
	} catch (...) {
		return -1;
	}
	return 0;
}
