/*
 * HTTPRequestHeader.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#include "HTTPDefines.h"
#include "HTTPRequestHeader.h"

#include <iostream>

HTTPRequestHeader::HTTPRequestHeader()
{
	_Init();
}


HTTPRequestHeader::HTTPRequestHeader(const HTTPRequestHeader& header)
{
	*this = header;
}


HTTPRequestHeader::HTTPRequestHeader(const std::string method,
		const std::string path, int majorVer, int minorVer)
{
	_Init();
	SetRequest(method, path);
}


HTTPRequestHeader::~HTTPRequestHeader()
{
}


std::string
HTTPRequestHeader::UserAgent() const
{
	return fUserAgent;
}


void
HTTPRequestHeader::SetUserAgent(const std::string string)
{
	fUserAgent = string;
}


std::string
HTTPRequestHeader::Method() const
{
	return fMethod;
}


std::string
HTTPRequestHeader::Path() const
{
	return fPath;
}


std::string
HTTPRequestHeader::ToString() const
{
	std::string host = fPath;
	std::string resource = "/";
	size_t pos = fPath.find("/");
	if (pos != std::string::npos) {
		host = fPath.substr(0, pos);
		resource = fPath.substr(pos, std::string::npos);
	}
	std::string string;
	string.append(fMethod).append(" ");
	string.append(resource).append(" ");
	string.append("HTTP/1.1").append(CRLF);
	string.append("Host: ").append(host);
	string.append(CRLF);
	string.append(fUserAgent).append(CRLF);

	string.append(HTTPHeader::ToString());

	return string;
}


void
HTTPRequestHeader::SetRequest(const std::string method,
		const std::string path, int majorVer, int minorVer)
{
	fMethod = method;
	fPath = path;
}


HTTPRequestHeader&
HTTPRequestHeader::operator=(const HTTPRequestHeader& header)
{
	SetRequest(header.fMethod, header.fPath);
	fUserAgent = header.fUserAgent;
	return *this;
}


void
HTTPRequestHeader::_Init()
{
	fUserAgent = "Borked HTTP Library";
}
