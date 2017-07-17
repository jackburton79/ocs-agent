/*
 * Socket.h
 *
 *  Created on: 12/07/2017
 *  Copyright 2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>

class Socket {
public:
	Socket();
	Socket(int domain, int type, int protocol);
	~Socket();

	int Open(int domain, int type, int protocol);
	void Close();

	bool IsOpened() const;

	int Connect(const struct sockaddr *address, socklen_t len);

	void SetOption(int level, int name, const void *value, socklen_t len);

	int Read(void* data, const size_t& length);
	int Write(const void* data, const size_t& length);

private:
	int fFD;
};

#endif // SOCKET_H
