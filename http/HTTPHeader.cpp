/*
 * HTTPHeader.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#include "HTTPDefines.h"
#include "HTTPHeader.h"

#include <cstdlib>
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
	fValues[key] = value;
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
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(HTTPContentLength);
	return i != fValues.end();
}


bool
HTTPHeader::HasContentType() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(HTTPContentType);
	return i != fValues.end();
}


bool
HTTPHeader::HasKey(const std::string key)
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(key);
	return i != fValues.end();
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
	// TODO: ATM works like AddValue().
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

	string.append(CRLF);

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


HTTPHeader&
HTTPHeader::operator =(const HTTPHeader& header)
{
	fValues = header.fValues;
	return *this;
}


void
HTTPHeader::_Init()
{
}
