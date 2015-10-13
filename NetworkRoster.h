/*
 * NetworkInterfaceRoster.h
 *
 *  Created on: 12 ott 2015
 *      Author: stefano
 */

#ifndef NETWORKROSTER_H_
#define NETWORKROSTER_H_

#include <netinet/in.h>
#include <net/route.h>
#include <string>

class NetworkInterface;
class NetworkRoster {
public:
	NetworkRoster();
	~NetworkRoster();

	int CountInterfaces(int family = AF_INET);
	int GetNextInterface(unsigned int* cookie, NetworkInterface& interface);

	int GetDefaultGateway(const char* interfaceName, std::string& gateway);
};

#endif /* NETWORKROSTER_H_ */
