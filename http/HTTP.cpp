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


HTTP::HTTP(const std::string string, int port)
	:
	fPort(-1),
	fFD(-1),
	fLastError(0)
{
	std::string hostName = HostFromConnectionString(string);
	fHost = hostName;
	fPort = port;
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


int
HTTP::Get(const std::string path)
{
	HTTPRequestHeader requestHeader("GET", path);

	return Request(requestHeader);
}


HTTPResponseHeader
HTTP::LastResponse() const
{
	return fLastResponse;
}


int
HTTP::SetHost(const std::string hostName, int port)
{
	fHost = HostFromConnectionString(hostName);
	fPort = port;
	fLastError = 0;

	return 0;
}


int
HTTP::Post(const std::string path, char* data)
{
	HTTPRequestHeader requestHeader("POST", path);

	return Request(requestHeader, data);
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
HTTP::Request(HTTPRequestHeader& header, const void* data, size_t length)
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
	_ReadLineFromSocket(replyString, fFD);

	std::string statusLine = replyString;
	int code;
	::sscanf(statusLine.c_str(), "HTTP/1.%*d %03d", (int*)&code);

	try {
		// TODO: Add a Clear() method
		fLastResponse = HTTPResponseHeader();
		fLastResponse.SetStatusLine(code, statusLine.c_str());
		while (_ReadLineFromSocket(replyString, fFD)) {
			size_t pos = replyString.find(":");
			fLastResponse.SetValue(replyString.substr(0, pos),
					replyString.substr(pos + 1, std::string::npos));
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
HTTP::_HandleConnectionIfNeeded(const std::string string, const int port)
{
	std::string hostName = HostFromConnectionString(string);

#if 0
	std::cout << "HTTP::_HandleConnectionIfNeeded(" << string << ", ";
	std::cout << port << ")" << std::endl;
#endif
	if (fFD >= 0) {
		if (hostName == "" || (hostName == fHost && port == fPort)) {
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

	::setsockopt(fFD, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

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
