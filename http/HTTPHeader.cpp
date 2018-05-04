/*
 * HTTPHeader.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#include "HTTPDefines.h"
#include "HTTPHeader.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

HTTPHeader::HTTPHeader()
{
	_Init();
}


HTTPHeader::HTTPHeader(const std::string string)
{
	_Init();
}


HTTPHeader::HTTPHeader(const HTTPHeader& header)
{
	_Init();
	*this = header;
}


HTTPHeader::~HTTPHeader()
{
}


void
HTTPHeader::AddValue(const std::string key, const std::string value)
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(HTTPContentLength);
	if ( i != fValues.end())
		fValues[key].append(", ");

	fValues[key] = fValues[key].append(value);
}


int
HTTPHeader::ContentLength() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(HTTPContentLength);
	if (i == fValues.end())
		return 0;

	return ::strtol(i->second.c_str(), NULL, 10);
}


std::string
HTTPHeader::ContentType() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(HTTPContentType);
	if (i == fValues.end())
		return ""; // TODO: Assume html/text ?

	return i->second;
}


bool
HTTPHeader::HasContentLength() const
{
	return fValues.find(HTTPContentLength) != fValues.end();
}


bool
HTTPHeader::HasContentType() const
{
	return fValues.find(HTTPContentType) != fValues.end();
}


bool
HTTPHeader::HasKey(const std::string key) const
{
	return fValues.find(key) != fValues.end();
}


void
HTTPHeader::SetContentLength(int len)
{
	std::ostringstream stream;
	stream << len;
	fValues[HTTPContentLength] = stream.str();
}


void
HTTPHeader::SetContentType(const std::string type)
{
	fValues[HTTPContentType] = type;
}


void
HTTPHeader::SetValue(const std::string key, const std::string value)
{
	fValues[key] = value;
}


std::string
HTTPHeader::ToString() const
{
	std::string string;

	std::map<std::string, std::string>::const_iterator i;
	for (i = fValues.begin(); i != fValues.end(); i++) {
		string.append(i->first);
		string.append(": ");
		string.append(i->second);
		string.append(CRLF);
	}

	return string;
}


std::string
HTTPHeader::Value(const std::string key) const
{
	std::map<std::string, std::string>::const_iterator i = fValues.find(key);
	if (i == fValues.end())
		return "";

	return i->second;
}


/* virtual */
void
HTTPHeader::Clear()
{
	fValues.clear();
}


HTTPHeader&
HTTPHeader::operator=(const HTTPHeader& header)
{
	fValues = header.fValues;
	return *this;
}


void
HTTPHeader::_Init()
{
}


// ICompareString
bool
ICompareString::operator()(const std::string a, const std::string b) const
{
	return ::strcasecmp(a.c_str(), b.c_str()) < 0;
}
