/*
 * SSLSocket.cpp
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#include "SSLSocket.h"

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>


static SSL_CTX* sSSLContext = NULL;


SSLSocket::SSLSocket()
	:
	fSSLConnection(NULL)
{
	if (sSSLContext == NULL)
		_SSLInit();
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
	if (fSSLConnection == NULL)
		return -1;
	if (!HostName().empty())
		SSL_set_tlsext_host_name(fSSLConnection, HostName().c_str());
	SSL_set_fd(fSSLConnection, FD());
	status = SSL_connect(fSSLConnection);
	if (status != 1) {
		// TODO: Maybe use SSL_get_error to retrieve the correct error, but
		// we shouldn't pass it to the upper layers, anyway
		return -1;
	}

	// Connection estabilished successfully.
	if (!_CheckCertificate())
        return -1;
	return 0;
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


void
SSLSocket::_SSLInit()
{
	if (sSSLContext == NULL) {
		SSL_load_error_strings();
		SSL_library_init();
		sSSLContext = SSL_CTX_new(SSLv23_client_method());
		if (sSSLContext == NULL)
			throw std::runtime_error("SSL: can't initialize SSL Library");
	}
}


bool
SSLSocket::_CheckCertificate()
{
#if 0
	X509 *cert = SSL_get_peer_certificate(fSSLConnection);
	if (cert == NULL)
		return false;
	STACK_OF(X509) *sk = SSL_get_peer_cert_chain(fSSLConnection);
	if (sk == NULL)
		return false;

	X509_NAME* subjectName = X509_get_subject_name(cert);
	if (subjectName == NULL)
		return false;
	char *subj = X509_NAME_oneline(subjectName, NULL, 0);
	X509_NAME* issuerName = X509_get_issuer_name(cert);
	if (issuerName == NULL)
		return false;
	char *issuer = X509_NAME_oneline(issuerName, NULL, 0);

	ASN1_TIME *notBefore = X509_get_notBefore(cert);
	ASN1_TIME *notAfter = X509_get_notAfter(cert);

	std::cout << "subject: " << subj << std::endl;
	std::cout << "issuer: " << issuer << std::endl;
#endif
	return true;
}
