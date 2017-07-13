/*
 * HTTPRequestHeader.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#include "HTTPDefines.h"
#include "HTTPRequestHeader.h"
#include "URL.h"

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
		const std::string url, int majorVer, int minorVer)
{
	_Init();
	SetRequest(method, url);
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
HTTPRequestHeader::URL() const
{
	return fURL;
}


std::string
HTTPRequestHeader::ToString() const
{
	std::string string;
	std::string host = fURL;
	std::string resource = "/";
	size_t pos = fURL.find("//");
	pos = fURL.find("/", pos + 2);
	if (pos != std::string::npos) {
		host = fURL.substr(0, pos);
		resource = fURL.substr(pos, std::string::npos);
	}
	string.append(fMethod).append(" ");
	string.append(resource).append(" ");
	string.append("HTTP/1.1").append(CRLF);
	if (HasKey(HTTPHost))
		string.append(HTTPHost).append(": ").append(Value(HTTPHost)).append(CRLF);
	string.append(HTTPUserAgent).append(": ").append(fUserAgent).append(CRLF);
	string.append(HTTPHeader::ToString());

	return string;
}


void
HTTPRequestHeader::SetRequest(const std::string method,
		const std::string url, int majorVer, int minorVer)
{
	fMethod = method;
	fURL = url;
	
	std::string hostName = ::URL(url.c_str()).Host();
	if (hostName != "")
		fValues["Host"] = hostName;
}



/* virtual */
void
HTTPRequestHeader::Clear()
{
	HTTPHeader::Clear();
	fMethod = "";
	fURL = "";

	_Init();
}


HTTPRequestHeader&
HTTPRequestHeader::operator=(const HTTPRequestHeader& header)
{
	HTTPHeader::operator=(header);

	SetRequest(header.fMethod, header.fURL);
	fUserAgent = header.fUserAgent;
	return *this;
}


void
HTTPRequestHeader::_Init()
{
	fUserAgent = "Borked HTTP Library";
}
