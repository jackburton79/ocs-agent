/*
 * SSLSocket.h
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include "Socket.h"

#include <openssl/ssl.h>

class SSLSocket : public Socket {
public:
	SSLSocket();
	SSLSocket(int domain, int type, int protocol);
	virtual ~SSLSocket();

	virtual int Open(int domain, int type, int protocol);
	virtual void Close();

	virtual int Connect(const struct sockaddr *address, socklen_t len);

	virtual int Read(void* data, const size_t& length);
	virtual int Write(const void* data, const size_t& length);

private:
	void SSLInit();

	SSL* fSSLConnection;
};

#endif // SSLSOCKET_H
