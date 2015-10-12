/*
 * Network.h
 *
 *  Created on: 12 ott 2015
 *      Author: stefano
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <string>

class NetworkInterface {
public:
	NetworkInterface();
	NetworkInterface(const char* name);
	~NetworkInterface();

	std::string HardwareAddress();
	std::string IPAddress();
	std::string NetMask();

private:

	int _DoRequest(int request, struct ifreq& ifr);
	std::string fName;
};

#endif /* NETWORKINTERFACE_H_ */
