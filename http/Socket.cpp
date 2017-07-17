/*
 * Socket.cpp
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */
 
#include "Socket.h"
 
#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


Socket::Socket()
	:
	fFD(-1)
{
}


Socket::~Socket()
{
    Close();
}


int
Socket::Open(int domain, int type, int protocol)
{
	if (fFD >= 0)
		return -1;
	fFD = ::socket(domain, type, protocol);
	return fFD;
}


void
Socket::Close()
{
	if (fFD >= 0) {
		::close(fFD);
		fFD = -1;
	}
}


int
Socket::FD() const
{
	return fFD;
}


bool
Socket::IsOpened() const
{
	return fFD >= 0;    
}


void
Socket::SetOption(int level, int name, const void *value, socklen_t len)
{
	::setsockopt(fFD, level, name, value, len);
}


int
Socket::Connect(const struct sockaddr *address, socklen_t addrLen)
{
	return ::connect(fFD, address, addrLen);
}


int
Socket::Read(void* data, const size_t& length)
{
	return ::read(fFD, data, length);
}
 
 
int
Socket::Write(const void* data, const size_t& length)
{
	return ::write(fFD, data, length);
}
