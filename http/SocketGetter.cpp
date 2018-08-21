/*
 * SocketGetter.h
 *
 *  Created on: 17/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */
 
#include "SocketGetter.h"

#include "Socket.h"
#include "SSLSocket.h"

#include <stdexcept>

SocketGetter::SocketGetter()
{
}


Socket*
SocketGetter::GetSocket(std::string protocol)
{
	if (protocol == "https")
		return new SSLSocket();
	else if (protocol == "http")
		return new Socket();
	
	throw std::runtime_error("INVALID PROTOCOL!!!!!!");
	return NULL;
}


Socket*
SocketGetter::GetSocket(std::string protocol, int domain, int type)
{
	if (protocol == "https")
		return new SSLSocket(domain, type, 0);
	else if (protocol == "http")
		return new Socket(domain, type, 0);

	throw std::runtime_error("INVALID PROTOCOL!!!!!!");
	return NULL;
}
