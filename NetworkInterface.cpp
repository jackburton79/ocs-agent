/*
 * Network.cpp
 *
 *  Created on: 12 ott 2015
 *      Author: stefano
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

#include <iostream>
#include <sstream>


NetworkInterface::NetworkInterface()
{

}


NetworkInterface::NetworkInterface(const char* name)
	:
	fName(name)
{
}


NetworkInterface::~NetworkInterface()
{
}


std::string
NetworkInterface::HardwareAddress()
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFHWADDR, ifr) != 0)
		return "";

	struct sockaddr* addr = (struct sockaddr*)&ifr.ifr_hwaddr;
	std::ostringstream stream;
	for (size_t i = 0; i < ETHER_ADDR_LEN; i++) {
		int byte = (addr->sa_data[i] & 0xFF);
		if (i != 0)
			stream << ":";
		stream << std::hex << byte;
	}

	return stream.str();
}


std::string
NetworkInterface::IPAddress()
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFADDR, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	return inet_ntoa(ipaddr->sin_addr);
}


std::string
NetworkInterface::NetMask()
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFNETMASK, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_netmask;
	return inet_ntoa(ipaddr->sin_addr);
}


int
NetworkInterface::_DoRequest(int request, struct ifreq& ifr)
{
	size_t ifNameLen = fName.size();
	if (ifNameLen < sizeof(ifr.ifr_name)) {
		::memcpy(ifr.ifr_name, fName.c_str(), ifNameLen);
		ifr.ifr_name[ifNameLen] = 0;
	} else {
		std::cerr << "interface name is too long" << std::endl;
	}

	int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1)
		return errno;

	int status = 0;
	if (ioctl(fd, request, &ifr) == -1)
		status = errno;

	::close(fd);

	return status;
}
