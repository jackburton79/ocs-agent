/*
 * Network.cpp
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#include "NetworkInterface.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <iomanip>
#include <iostream>
#include <sstream>


NetworkInterface::NetworkInterface()
{
}


NetworkInterface::NetworkInterface(const char* name)
	:
	fName(name)
{
	if (fName.size() > IFNAMSIZ)
		throw "NetworkInterface::NetworkInterface(): Name too long";
}


NetworkInterface::~NetworkInterface()
{
}


std::string
NetworkInterface::Name() const
{
	return fName;
}


std::string
NetworkInterface::HardwareAddress() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFHWADDR, ifr) != 0)
		return "";

	struct sockaddr* addr = (struct sockaddr*)&ifr.ifr_hwaddr;
	std::ostringstream stream;
	for (size_t i = 0; i < ETHER_ADDR_LEN; i++) {
		int byte = addr->sa_data[i] & 0xFF;
		if (i != 0)
			stream << ":";
		stream << std::hex << std::setw(2) << std::setfill('0') << byte;
	}

	return stream.str();
}


std::string
NetworkInterface::IPAddress() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFADDR, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	return inet_ntoa(ipaddr->sin_addr);
}


std::string
NetworkInterface::NetMask() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFNETMASK, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_netmask;
	return inet_ntoa(ipaddr->sin_addr);
}


std::string
NetworkInterface::BroadcastAddress() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFBRDADDR, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_broadaddr;
	return inet_ntoa(ipaddr->sin_addr);
}


std::string
NetworkInterface::Type() const
{
	// TODO:
	return "";
}


std::string
NetworkInterface::Speed() const
{
	return "";
}


std::string
NetworkInterface::Status() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFFLAGS, ifr) != 0)
		return "";

	return ifr.ifr_flags & IFF_UP ? "Up" : "Down";
}


int
NetworkInterface::_DoRequest(int request, struct ifreq& ifr)  const
{
	size_t ifNameLen = fName.size();
	::memcpy(ifr.ifr_name, fName.c_str(), ifNameLen);
	ifr.ifr_name[ifNameLen] = 0;

	int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1)
		return errno;

	int status = 0;
	if (ioctl(fd, request, &ifr) == -1)
		status = errno;

	::close(fd);

	return status;
}
