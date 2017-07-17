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
#include "Socket.h"
#include "SSLSocket.h"
#include "Support.h"
#include "URL.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

HTTP::HTTP()
	:
	fPort(-1),
	fSocket(NULL),
	fLastError(0)
{
}


HTTP::HTTP(const std::string string)
	:
	fPort(-1),
	fSocket(NULL),
	fLastError(0)
{
	URL url(string.c_str());
	fHost = url.Host();
	fPort = url.Port();
}


HTTP::~HTTP()
{
	Close();
	delete fSocket;
}


void
HTTP::Close()
{
	if (fSocket != NULL) {
		fSocket->Close();
		delete fSocket;
		fSocket = NULL;
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
	try {
		URL url(hostName.c_str());
		fHost = url.Host();
		fPort = url.Port();
		fLastError = 0;
	} catch (...) {
		fLastError = -1;
	}
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
	if (fSocket->Read(data, length) != (int)length) {
		fLastError = errno;
		return errno;
	}

	return length;
}


int
HTTP::Request(const HTTPRequestHeader& header, const void* data, const size_t length)
{
	if (!_HandleConnectionIfNeeded(header.URL()))
		return -1;

	fCurrentRequest = header;
	if (fCurrentRequest.URL() == "")
		fCurrentRequest.SetValue(HTTPHost, fHost);

	std::string string = fCurrentRequest.ToString().append(CRLF);

	if (fSocket->Write(string.c_str(), string.length())
			!= (int)string.length()) {
		fLastError = errno;
		return errno;
	}

	if (data != NULL && length != 0) {
		if (fSocket->Write(data, length) != (int)length) {
			fLastError = errno;
			return errno;
		}
	}

	std::string replyString;
	try {
		_ReadLineFromSocket(replyString, fSocket);
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
		while (_ReadLineFromSocket(replyString, fSocket)) {
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


/* static */
std::string
HTTP::Base64Encode(std::string string)
{
	// TODO: Error checking
	BIO* b64f = BIO_new(BIO_f_base64());
	BIO* buffer = BIO_new(BIO_s_mem());
	buffer = BIO_push(b64f, buffer);

	BIO_set_flags(buffer, BIO_FLAGS_BASE64_NO_NL);
	BIO_set_close(buffer, BIO_CLOSE);
	BIO_write(buffer, string.c_str(), string.length());
	BIO_flush(buffer);

	BUF_MEM *pointer;
	BIO_get_mem_ptr(buffer, &pointer);

	size_t encodedSize = pointer->length;
	std::string encoded(encodedSize + 1, '\0');
	memcpy(&encoded[0], pointer->data, encodedSize);
	encoded.resize(encodedSize);

	BIO_free_all(buffer);

	return encoded;
}


bool
HTTP::_HandleConnectionIfNeeded(const std::string string)
{
	URL url(string.c_str());
	std::string hostName = url.Host();
	int port = url.Port();
	if (url.IsRelative()) {
		// url is relative, reuse the old host/port
		hostName = fHost;
		port = fPort;
	}

	if (hostName == "")
		return false;

	// Check if we are already connected to this server,
	// so we can reuse the existing connection
	if (fSocket != NULL && fSocket->IsOpened()) {
		if (hostName == fHost && port == fPort) {
			// But not if the server closed it from its side
			HTTPResponseHeader lastResponse = LastResponse();
			if (!lastResponse.HasKey("connection")
				|| lastResponse.Value("connection") != "close") {
				return true;
			}
		}
		// Different server, or same server, but connection closed
		delete fSocket;
		fSocket = NULL;
	}

	fHost = hostName;
	fPort = port;

	struct hostent* hostEnt = ::gethostbyname(fHost.c_str());
	if (hostEnt == NULL) {
		fLastError = h_errno;
		return false;
	}

	try {
		if (url.Protocol() == "https") {
			// TODO: Handle this differently
			if (fPort == 80)
				fPort = 443;
			fSocket = new SSLSocket();
		}
		else
			fSocket = new Socket();
	} catch (...) {
		return false;
	}
	
	if ((fSocket->Open(AF_INET, SOCK_STREAM, 0)) < 0) {
		fLastError = errno;
		delete fSocket;
		fSocket = NULL;
		return false;
	}

	struct timeval tv;
	tv.tv_sec = 15;
	tv.tv_usec = 0;

	fSocket->SetOption(SOL_SOCKET, SO_KEEPALIVE, 0, 0);
	fSocket->SetOption(SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	fSocket->SetOption(SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));

	struct sockaddr_in serverAddr;
	::memset((char*)&serverAddr, 0, sizeof(serverAddr));
	::memcpy((char*)&serverAddr.sin_addr, hostEnt->h_addr, hostEnt->h_length);
	serverAddr.sin_family = hostEnt->h_addrtype;
	serverAddr.sin_port = (unsigned short)htons(fPort);

	if (fSocket->Connect((const struct sockaddr*)&serverAddr,
			sizeof(serverAddr)) < 0) {
		fLastError = errno;
		return false;
	}

	fLastError = 0;

	return true;
}


/* static */
bool
HTTP::_ReadLineFromSocket(std::string& string, Socket* socket)
{
	std::ostringstream s;
	char byte;
	int sizeRead = 0;
	while ((sizeRead = socket->Read(&byte, 1)) > 0) {
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
