/*
 * NetworkInterfaceRoster.h
 *
 *  Created on: 12 ott 2015
 *      Author: stefano
 */

#ifndef NETWORKROSTER_H_
#define NETWORKROSTER_H_

#include <netinet/in.h>

class NetworkInterface;
class NetworkRoster {
public:
	NetworkRoster();
	~NetworkRoster();

	int CountInterfaces(int family = AF_UNSPEC);
	int GetNextInterface(unsigned int* cookie, NetworkInterface& interface);
};

#endif /* NETWORKROSTER_H_ */
