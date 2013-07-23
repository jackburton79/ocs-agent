/*
 * HTTP.cpp
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#include "HTTP.h"
#include "HTTPRequestHeader.h"
#include "HTTPResponseHeader.h"

#include <sys/socket.h>

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

HTTP::HTTP()
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
}


HTTP::HTTP(const std::string hostName, int port)
{
	SetHost(hostName, port);
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
	// TODO:
	return "";
}


int
HTTP::Get(const std::string path)
{
	if (!_ConnectIfNeeded())
		return -1;

	return -1;
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
	if (!_ConnectIfNeeded())
		return -1;
	return -1;
}


int
HTTP::SetHost(const std::string hostName, int port)
{
	fHost = hostName;
	fPort = port;

	return 0;
}


int
HTTP::_ConnectIfNeeded()
{
	// TODO: improve error checking
	if (fFD < 0) {
		struct hostent* hostEnt = ::gethostbyname(fHost.c_str());
		struct sockaddr_in serverAddr;

		::memset((char*)&serverAddr,0, sizeof(serverAddr));
		::memcpy((char*)&serverAddr.sin_addr, hostEnt->h_addr, hostEnt->h_length);
		serverAddr.sin_family = hostEnt->h_addrtype;
		serverAddr.sin_port = (unsigned short)htons(fPort);

		if ((fFD = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
			return -1;

		::setsockopt(fFD, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

		if (::connect(fFD, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
			return -1;
	}

	return fFD;
}
