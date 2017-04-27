/*
 * NetworkInterfaceRoster.cpp
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#include "NetworkRoster.h"
#include "NetworkInterface.h"
#include "Support.h"

#include <errno.h>
#include <ifaddrs.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <cstdlib>
#include <iostream>
#include <set>


NetworkRoster::NetworkRoster()
{
}


NetworkRoster::~NetworkRoster()
{
}


int
NetworkRoster::CountInterfaces(int family)
{
	ifaddrs* addrs = NULL;
	if (getifaddrs(&addrs) != 0)
		return errno;

	std::set<std::string> interfaces;
	ifaddrs* addr = addrs;
	while (addr != NULL) {
		interfaces.insert(addr->ifa_name);
		addr = addr->ifa_next;
	}
	freeifaddrs(addrs);

	return interfaces.size();
}


int
NetworkRoster::GetNextInterface(unsigned int* cookie, NetworkInterface& interface)
{
	if (cookie == NULL)
		return -1;

	int index = *cookie;
	index++;

	ifreq ifr;
	ifr.ifr_ifindex = index;

	int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1)
		return errno;

	if (::ioctl(fd, SIOCGIFNAME, &ifr) == -1) {
		::close(fd);
		return errno;
	}

	::close(fd);

	interface = NetworkInterface(ifr.ifr_name);

	*cookie = index;

	return 0;
}
