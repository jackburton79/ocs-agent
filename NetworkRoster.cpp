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
	_RefreshInterfaces();
}


NetworkRoster::~NetworkRoster()
{
}


int
NetworkRoster::CountInterfaces(int family)
{
	_RefreshInterfaces();
	return fInterfaces.size();
}


int
NetworkRoster::GetNextInterface(unsigned int* cookie, NetworkInterface& interface)
{
	if (cookie == NULL)
		return -1;

	int index = *cookie;
	if ((size_t)index >= fInterfaces.size())
		return -1;
		
	interface = NetworkInterface(fInterfaces[index].c_str());
	
	*cookie = index + 1;

	return 0;
}


int
NetworkRoster::_RefreshInterfaces()
{
	fInterfaces.clear();
	
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

	std::set<std::string>::const_iterator i;
	for (i = interfaces.begin(); i != interfaces.end(); i++)
		fInterfaces.push_back((*i));

	return 0;	
}