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


int
NetworkRoster::GetDefaultGateway(const char* interfaceName,
		std::string& defaultGateway)
{
	popen_streambuf routeBuf("export LC_ALL=C; route -n", "r");
	std::istream stream(&routeBuf);

	try {
		std::string line;
		std::getline(stream, line); // Skip Title

		// header
		std::getline(stream, line);
		size_t gatewayPos = line.find("Gateway");
		size_t maskPos = line.find("Genmask");

		while (std::getline(stream, line) > 0) {
			if (line.find(interfaceName) == std::string::npos)
				continue;

			std::string destination = line.substr(0, gatewayPos);
			trim(destination);
			std::string gateway = line.substr(gatewayPos, maskPos - gatewayPos);
			trim(gateway);

			if (gateway == "0.0.0.0") {
				//i->network = destination;
			} else if (destination == "0.0.0.0") {
				defaultGateway = gateway;
				break;
			}
		}
	} catch (...) {

	}
	return true;
}


