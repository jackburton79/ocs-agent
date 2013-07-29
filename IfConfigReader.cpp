/*
 * IfConfigReader.cpp
 *
 *  Created on: 18/lug/2013
 *      Author: stefano
 */

#include "IfConfigReader.h"
#include "Support.h"

#include <iostream>
#include <sstream>

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

	popen_streambuf routeBuf("export LC_ALL=C; route -n", "r");
	std::istream routeStream(&routeBuf);
	network_info info;

	_ReadRouteInfo(info, routeStream);

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


bool
IfConfigReader::_ReadRouteInfo(network_info& info, std::istream& stream)
{
	// TODO: Highly inefficient. Improve
	try {
		std::string line;
		std::getline(stream, line); // Skip Title

		// header
		std::getline(stream, line);
		size_t gatewayPos = line.find("Gateway");
		size_t maskPos = line.find("Genmask");

		while (std::getline(stream, line) > 0) {
			Rewind();
			network_info networkInfo;
			std::list<network_info>::iterator i;
			for (i = fItems.begin(); i != fItems.end(); i++) {
				if (line.find((*i).description) == std::string::npos)
					continue;

				std::string destination = line.substr(0, gatewayPos);
				trim(destination);
				std::string gateway = line.substr(gatewayPos, maskPos - gatewayPos);
				trim(gateway);

				if (gateway == "0.0.0.0") {
					i->network = destination;
				} else if (destination == "0.0.0.0") {
					i->gateway = gateway;
					break;
				}
			}
		}
	} catch (...) {

	}
	return true;
}
