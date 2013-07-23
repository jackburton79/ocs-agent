/*
 * HTTP.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#include "HTTP.h"
#include "HTTPDefines.h"
#include "HTTPRequestHeader.h"
#include "HTTPResponseHeader.h"

#include <sys/socket.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

HTTP::HTTP()
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
}


HTTP::HTTP(const std::string hostName, int port)
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
	Connect(hostName, port);
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
	return HTTPRequestHeader();
}


int
HTTP::Error() const
{
	return fLastError;
}


std::string
HTTP::ErrorString() const
{
	return strerror(fLastError);
}


int
HTTP::Get(const std::string path)
{
	if (fFD < 0)
		return -1;

	std::string fullPath = fHost + path;
	HTTPRequestHeader requestHeader("GET", fullPath);
	std::string string = requestHeader.ToString();

	std::cout << "HTTP::GET: header:" << std::endl << string << std::endl;
	if (::write(fFD, string.c_str(), string.length()) != (int)string.length()) {
		std::cerr << "GET failed: " << std::endl;
		fLastError = -1;
		return -1;
	}

	return 0;
}


HTTPResponseHeader
HTTP::LastResponse() const
{
	// TODO:
	return HTTPResponseHeader();
}


int
HTTP::Post(const std::string path, char* data)
{
	if (fFD < 0)
		return -1;
	return -1;
}


int
HTTP::Connect(const std::string string, int port)
{
	// TODO: Return an "already connected" error, or close and open a new
	// connection
	if (fFD >= 0) {
		std::cerr << "HTTP::Connect: already connected" << std::endl;
		return -1;
	}

	std::string hostName = _HostFromConnectionString(string);

	fHost = hostName;
	fPort = port;

	struct hostent* hostEnt = ::gethostbyname(fHost.c_str());
	if (hostEnt == NULL) {
		fLastError = h_errno;
		return fLastError;
	}

	if ((fFD = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fLastError = errno;
		return fLastError;
	}

	::setsockopt(fFD, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

	struct sockaddr_in serverAddr;
	::memset((char*)&serverAddr,0, sizeof(serverAddr));
	::memcpy((char*)&serverAddr.sin_addr, hostEnt->h_addr, hostEnt->h_length);
	serverAddr.sin_family = hostEnt->h_addrtype;
	serverAddr.sin_port = (unsigned short)htons(fPort);
	if (::connect(fFD, (const struct sockaddr*)&serverAddr,
			sizeof(serverAddr)) < 0) {
		fLastError = errno;
		return fLastError;
	}

	fLastError = 0;
	return fFD;
}


std::string
HTTP::_HostFromConnectionString(std::string string) const
{
	// TODO: Remove port if specified
	size_t prefixPos = string.find(HTTPProtocolPrefix);
	if (prefixPos == std::string::npos)
		return string;

	return string.substr(HTTPProtocolPrefix.length(), std::string::npos);
}
