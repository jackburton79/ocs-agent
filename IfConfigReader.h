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

#include "ItemsList.h"

struct network_info {
	std::string description;
	std::string type;
	int speed;
	std::string mac_address;
	std::string status;
	std::string ip_address;
	std::string netmask;
	std::string gateway;
	std::string network;
	std::string dhcp_ip;
};


class IfConfigReader : public ItemsList<network_info>{
public:
	IfConfigReader();
	~IfConfigReader();

private:

	bool _ReadNetworkInfo(network_info& info, std::istream& stream);

};

#endif /* IFCONFIGREADER_H_ */
