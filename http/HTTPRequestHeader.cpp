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
	std::string string;
	std::string host = fPath;
	std::string resource = "/";
	size_t pos = fPath.find("//");
	pos = fPath.find("/", pos + 2);
	if (pos != std::string::npos) {
		host = fPath.substr(0, pos);
		resource = fPath.substr(pos, std::string::npos);
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
		const std::string path, int majorVer, int minorVer)
{
	fMethod = method;
	fPath = path;

	std::string hostName = HostFromConnectionString(path);
	if (hostName != "")
		fValues["Host"] = hostName;
}



/* virtual */
void
HTTPRequestHeader::Clear()
{
	HTTPHeader::Clear();
	fMethod = "";
	fPath = "";

	_Init();
}


HTTPRequestHeader&
HTTPRequestHeader::operator=(const HTTPRequestHeader& header)
{
	HTTPHeader::operator=(header);

	SetRequest(header.fMethod, header.fPath);
	fUserAgent = header.fUserAgent;
	return *this;
}


void
HTTPRequestHeader::_Init()
{
	fUserAgent = "Borked HTTP Library";
}
