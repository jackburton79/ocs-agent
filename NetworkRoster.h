/*
 * NetworkInterfaceRoster.h
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#ifndef NETWORKROSTER_H_
#define NETWORKROSTER_H_

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <vector>

class NetworkInterface;
class NetworkRoster {
public:
	NetworkRoster();
	~NetworkRoster();

	int CountInterfaces(int family = AF_INET);
	int GetNextInterface(unsigned int* cookie, NetworkInterface& interface);

private:
	std::vector<std::string> fInterfaces;
	
	int _RefreshInterfaces();
};

#endif /* NETWORKROSTER_H_ */
