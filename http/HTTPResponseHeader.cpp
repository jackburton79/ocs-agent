/*
 * HTTPResponseHeader.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#include "HTTPResponseHeader.h"

#include <iostream>

HTTPResponseHeader::HTTPResponseHeader()
	:
	fCode(0)
{
}


HTTPResponseHeader::HTTPResponseHeader(int code, const std::string text,
		const int majVersion, const int minVersion)
{
	SetStatusLine(code, text, majVersion, minVersion);
}


HTTPResponseHeader::HTTPResponseHeader(const HTTPResponseHeader& header)
{
	*this = header;
}


HTTPResponseHeader::~HTTPResponseHeader()
{
}


std::string
HTTPResponseHeader::ReasonPhrase() const
{
	return fText;
}


void
HTTPResponseHeader::SetStatusLine(int code, const std::string text,
		const int majVersion, const int minVersion)
{
	fCode = code;
	fText = text;
}


int
HTTPResponseHeader::StatusCode() const
{
	return fCode;
}


std::string
HTTPResponseHeader::ToString() const
{
	// TODO:
	std::string string;
	string.append(HTTPHeader::ToString());
	string.append(fText);
	return string;
}


HTTPResponseHeader&
HTTPResponseHeader::operator=(const HTTPResponseHeader& header)
{
	fText = header.fText;
	fCode = header.fCode;
	return *this;
}
