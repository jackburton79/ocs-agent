/*
 * URL.cpp
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#include <cstdlib>
#include <cstring>

const static char* kProtocolSuffix = "://";

#include "URL.h"

URL::URL()
	:
	fURLString(""),
	fProtocol(""),
	fHost(""),
	fPort(-1),
	fPath("")
{
}


URL::URL(const char* url)
	:
	fURLString(url),
	fProtocol(""),
	fHost(""),
	fPort(-1),
	fPath("")
{
	_DecodeURLString(url);
}


void
URL::SetTo(const char* url)
{
	fURLString = url;
	fProtocol = "";
	fHost = "";
	fPort = -1;
	fPath = "";
	_DecodeURLString(url);
}


std::string
URL::URLString() const
{
	return fURLString;
}


std::string
URL::Protocol() const
{
	return fProtocol;
}


std::string
URL::Host() const
{
	return fHost;
}


int
URL::Port() const
{
	return fPort;
}


std::string
URL::Path() const
{
	return fPath;
}


void
URL::_DecodeURLString(const char* url)
{
	// TODO: Handle malformed urls
	std::string string = url;
	std::string result = string;
	size_t suffixPos = string.find(kProtocolSuffix);	
	if (suffixPos != std::string::npos) {
		// Remove protocol part (<proto>://)
		fProtocol = string.substr(0, suffixPos);
		result = string.substr(suffixPos + strlen(kProtocolSuffix), -1);
	}
	size_t portPos = result.find(":");
	if (portPos != std::string::npos) {
		fHost = result.substr(0, portPos);
		size_t slashPos = result.find("/", portPos);
		if (slashPos != std::string::npos)
			fPath = result.substr(slashPos, -1);
		
		fPort = ::strtol(result.substr(portPos + 1, result.length()).c_str(),
			NULL, 10);
	} else {
		fPort = 80;
		size_t slashPos = result.find("/");
		if (slashPos != std::string::npos) {
			fHost = result.substr(0, slashPos);
			fPath = result.substr(slashPos, -1);
		} else
			fHost = result;
	}
}
