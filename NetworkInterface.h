/*
 * Network.h
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <string>

class NetworkInterface {
public:
	NetworkInterface();
	NetworkInterface(const char* name);
	~NetworkInterface();

	std::string Name() const;
	std::string HardwareAddress() const;
	std::string IPAddress() const;
	std::string NetMask() const;
	std::string BroadcastAddress() const;
	std::string Type() const;
	std::string Speed() const;
	std::string Status() const;

private:

	int _DoRequest(int request, struct ifreq& ifr) const;
	std::string fName;
};

#endif /* NETWORKINTERFACE_H_ */
