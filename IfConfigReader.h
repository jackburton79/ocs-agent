/*
 * IfConfigReader.h
 *
 *  Created on: 18/lug/2013
 *      Author: stefano
 */

#ifndef IFCONFIGREADER_H_
#define IFCONFIGREADER_H_

#include <list>
#include <sstream>
#include <string>


struct network_info {
	std::string description;
	std::string type;
	int speed;
	std::string mac_address;
	int status;
	std::string ip_address;
	std::string netmask;
	std::string gateway;
	std::string network;
	std::string dhcp_ip;
};


class IfConfigReader {
public:
	IfConfigReader();
	~IfConfigReader();

	void Rewind();
	bool GetNext(network_info& info);

private:

	bool _ReadNetworkInfo(network_info& info, std::istream& stream);

	std::list<network_info> fNetworks;
	std::list<network_info>::iterator fIterator;
};

#endif /* IFCONFIGREADER_H_ */
