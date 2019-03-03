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
SocketGetter::GetSocket(const std::string& protocol)
{
	if (protocol == "https")
		return new SSLSocket();
	else if (protocol == "http")
		return new Socket();
	
	throw std::runtime_error("INVALID PROTOCOL!!!!!!");
	return NULL;
}
