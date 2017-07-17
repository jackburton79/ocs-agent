/*
 * SSLSocket.cpp
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */
 
#include "SSLSocket.h"
 
#include <openssl/ssl.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


static SSL_CTX *sSSLContext = NULL;


SSLSocket::SSLSocket()
	:
	fSSLConnection(NULL)
{
	if (sSSLContext == NULL)
		SSLInit();
}


SSLSocket::~SSLSocket()
{
	Close();
}


int
SSLSocket::Open(int domain, int type, int protocol)
{
	return Socket::Open(domain, type, protocol);
}


void
SSLSocket::Close()
{
	if (fSSLConnection != NULL) {
		SSL_shutdown(fSSLConnection);
		SSL_free(fSSLConnection);
		fSSLConnection = NULL;
	}
	Socket::Close();
}


int
SSLSocket::Connect(const struct sockaddr *address, socklen_t addrLen)
{
	int status = Socket::Connect(address, addrLen);
	if (status != 0)
		return status;

	fSSLConnection = SSL_new(sSSLContext);
	if (fSSLConnection != NULL) {
		SSL_set_fd(fSSLConnection, FD());
		status = SSL_connect(fSSLConnection);
		return status;
	}

	// TODO: delete connection
	return -1;
}


int
SSLSocket::Read(void* data, const size_t& length)
{
	return SSL_read(fSSLConnection, data, length);
}
 
 
int
SSLSocket::Write(const void* data, const size_t& length)
{
	return SSL_write(fSSLConnection, data, length);
}


int
SSLSocket::SSLInit()
{
	if (sSSLContext == NULL) {
		SSL_load_error_strings();
		SSL_library_init();
		sSSLContext = SSL_CTX_new(SSLv23_client_method());
	}
	return sSSLContext != NULL ? 0 : -1;
}
