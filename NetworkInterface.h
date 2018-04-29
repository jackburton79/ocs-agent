/*
 * Network.h
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <list>
#include <string>
#include <net/if.h>
#include <netinet/in.h>

struct route_info {
	struct in_addr dstAddr;
	struct in_addr srcAddr;
	struct in_addr gateway;
	char ifName[IF_NAMESIZE];
};

class NetworkInterface {
public:
	NetworkInterface();
	NetworkInterface(const char* name);
	~NetworkInterface();

	std::string Name() const;
	std::string HardwareAddress() const;
	std::string IPAddress() const;
	std::string NetMask() const;
	std::string Network() const;
	std::string BroadcastAddress() const;
	std::string DefaultGateway() const;
	std::string Type() const;
	std::string Speed() const;
	std::string Status() const;

	bool IsLoopback() const;

private:
	int _GetRoutes(std::list<route_info>& routeList) const;
	int _DoRequest(int request, struct ifreq& ifr) const;

	std::string fName;
};

#endif /* NETWORKINTERFACE_H_ */
