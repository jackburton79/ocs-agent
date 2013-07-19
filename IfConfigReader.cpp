/*
 * IfConfigReader.cpp
 *
 *  Created on: 18/lug/2013
 *      Author: stefano
 */

#include "IfConfigReader.h"
#include "Support.h"

#include <iostream>

IfConfigReader::IfConfigReader()
{
	popen_streambuf ifc("export LC_ALL=C; ifconfig -a", "r");
	std::istream stream(&ifc);

	try {
		network_info networkInfo;
		while (_ReadNetworkInfo(networkInfo, stream)) {
			fItems.push_back(networkInfo);
		}
	} catch (...) {

	}

	Rewind();
}


IfConfigReader::~IfConfigReader()
{
}


bool
IfConfigReader::_ReadNetworkInfo(network_info& info, std::istream& stream)
{
	std::string line;
	std::getline(stream, line);

	try {
		info.description = line.substr(0, line.find(" "));
	} catch (...) {
		return false;
	}
	try {
		info.mac_address = line.substr(line.find("HWaddr"), std::string::npos);
		info.mac_address = info.mac_address.substr(info.mac_address.find(" "), std::string::npos);
		info.mac_address = trim(info.mac_address);
	} catch (...) {
		// No MAC
		info.mac_address = "";
	}


	std::getline(stream, line);
	if (line.find("inet addr:") != std::string::npos) {
		size_t mask;
		size_t bcast;
		try {
			mask = line.find("Mask");
			bcast = line.find("Bcast");
			info.netmask = line.substr(line.find(':', mask) + 1, bcast - 1);

		} catch (...) {
			// no netmask or broadcast
			info.netmask = "";
		}
		try {
			size_t colon = line.find(":");
			info.ip_address = line.substr(colon + 1, std::min(mask, bcast) - colon - 3);
			//info.ip_address = info.ip_address.substr(line.find(":") + 1, std::string::npos);

		} catch (...) {
			info.ip_address = "";
		}

		std::getline(stream, line);
	}

	if (line.find("inet6 addr") != std::string::npos)
		std::getline(stream, line);

	if (line.find("UP") != std::string::npos) {
		info.status = "Up";
	} else
		info.status = "Down";

	while (std::getline(stream, line) > 0) {
		if (line == "")
			return true;
		;
	}

	return false;

}
