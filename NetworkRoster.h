/*
 * NetworkInterfaceRoster.h
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#ifndef NETWORKROSTER_H_
#define NETWORKROSTER_H_

#include <netinet/in.h>
//#include <net/route.h>
#include <string>
#include <vector>

class NetworkInterface;
class NetworkRoster {
public:
	NetworkRoster();
	~NetworkRoster();

	int CountInterfaces(int family);
	int GetNextInterface(unsigned int* cookie, NetworkInterface& interface);

private:
	std::vector<std::string> fInterfaces;
	
	int _RefreshInterfaces();
};

#endif /* NETWORKROSTER_H_ */
