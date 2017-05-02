/*
 * HTTP.cpp
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#include "HTTP.h"
#include "HTTPDefines.h"
#include "HTTPRequestHeader.h"
#include "HTTPResponseHeader.h"
#include "Support.h"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

HTTP::HTTP()
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
}


HTTP::HTTP(const std::string string)
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
	if (GetHostAndPortFromString(string, fHost, fPort) != 0)
		throw "HTTP Initialize error";
}


HTTP::~HTTP()
{
	Close();
}


void
HTTP::Close()
{
	if (fFD > 0) {
		::close(fFD);
		fFD = -1;
	}
}


void
HTTP::ClearPendingRequests()
{
}


HTTPRequestHeader
HTTP::CurrentRequest() const
{
	return fCurrentRequest;
}


int
HTTP::Error() const
{
	return fLastError;
}


std::string
HTTP::ErrorString() const
{
	return ::strerror(fLastError);
}


HTTPResponseHeader
HTTP::LastResponse() const
{
	return fLastResponse;
}


int
HTTP::SetHost(const std::string hostName)
{
	fLastError = GetHostAndPortFromString(hostName, fHost, fPort);
	return fLastError;
}


int
HTTP::Get(const std::string path)
{
	HTTPRequestHeader requestHeader("GET", path);

	return Request(requestHeader);
}


int
HTTP::Put(const std::string path, const char* data, const size_t dataLength)
{
	HTTPRequestHeader requestHeader("PUT", path);

	return Request(requestHeader, data, dataLength);
}


int
HTTP::Post(const std::string path, const char* data, const size_t dataLength)
{
	HTTPRequestHeader requestHeader("POST", path);

	return Request(requestHeader, data, dataLength);
}


int
HTTP::Read(void* data,  const size_t& length)
{
	if (::read(fFD, data, length) != (int)length) {
		fLastError = errno;
		return errno;
	}

	return length;
}


int
HTTP::Request(const HTTPRequestHeader& header, const void* data, const size_t length)
{
	if (!_HandleConnectionIfNeeded(header.Path()))
		return -1;

	fCurrentRequest = header;
	if (fCurrentRequest.Path() == "")
		fCurrentRequest.SetValue(HTTPHost, fHost);

	std::string string = fCurrentRequest.ToString().append(CRLF);

#if 0
	std::cout << string << std::endl;
#endif
	if (::write(fFD, string.c_str(), string.length())
			!= (int)string.length()) {
		fLastError = errno;
		return errno;
	}

	if (data != NULL && length != 0) {
		if (::write(fFD, data, length) != (int)length) {
			fLastError = errno;
			return errno;
		}
	}
	
	std::string replyString;
	try {
		_ReadLineFromSocket(replyString, fFD);
	} catch (int error) {
		fLastError = error;
		return error;
	}

	std::string statusLine = replyString;
	int code;
	::sscanf(statusLine.c_str(), "HTTP/1.%*d %03d", (int*)&code);
	try {
		fLastResponse.Clear();
		fLastResponse.SetStatusLine(code, statusLine.c_str());
		while (_ReadLineFromSocket(replyString, fFD)) {
			size_t pos = replyString.find(":");
			std::string value = replyString.substr(pos + 1, std::string::npos);
			trim(value);
			fLastResponse.SetValue(replyString.substr(0, pos), value);
		}
	} catch (int error) {
		fLastError = error;
		return error;
	} catch (...) {
		return -1;
	}

	return 0;
}


bool
HTTP::_HandleConnectionIfNeeded(const std::string string)
{
	std::string hostName;
	int port;
	GetHostAndPortFromString(string, hostName, port);

#if 0
	std::cout << "HTTP::_HandleConnectionIfNeeded(" << string << ", ";
	std::cout << port << ")" << std::endl;
#endif
	if (fFD >= 0) {
		if (hostName == "" || (hostName == fHost && port == fPort)) {
			// we can reuse the existing connection,
			// unless the server closed it already.
			HTTPResponseHeader lastResponse = LastResponse();
			if (!lastResponse.HasKey("connection")
				|| lastResponse.Value("connection") != "close") 
		 		return true;
		}
		::close(fFD);
		fFD = -1;
	}

	if (hostName != "") {
		fHost = hostName;
		fPort = port;
	}

	struct hostent* hostEnt = ::gethostbyname(fHost.c_str());
	if (hostEnt == NULL) {
		fLastError = h_errno;
		return false;
	}

	if ((fFD = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fLastError = errno;
		return false;
	}

	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;

	::setsockopt(fFD, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
	::setsockopt(fFD, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	::setsockopt(fFD, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	
	struct sockaddr_in serverAddr;
	::memset((char*)&serverAddr, 0, sizeof(serverAddr));
	::memcpy((char*)&serverAddr.sin_addr, hostEnt->h_addr, hostEnt->h_length);
	serverAddr.sin_family = hostEnt->h_addrtype;
	serverAddr.sin_port = (unsigned short)htons(fPort);

	if (::connect(fFD, (const struct sockaddr*)&serverAddr,
			sizeof(serverAddr)) < 0) {
		fLastError = errno;
		return false;
	}

	fLastError = 0;

	return true;
}


/* static */
bool
HTTP::_ReadLineFromSocket(std::string& string, int socket)
{
	std::ostringstream s;
	char byte;
	int sizeRead = 0;
	while ((sizeRead = ::read(socket, &byte, 1)) > 0) {
		if (byte == '\012')
			break;
		if (byte != '\015')
			s.write(&byte, sizeRead);
	}

	if (sizeRead < 0)
		throw errno;

	string = s.str();

	if (string == "")
		return false;

	return true;
}
