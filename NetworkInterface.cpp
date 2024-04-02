/*
 * Network.cpp
 *
 *  Created on: 12 ott 2015
 *      Author: Stefano Ceccherini
 */

#include "NetworkInterface.h"
#include "Support.h"

#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>

#include <sys/ioctl.h>

#include <iomanip>
#include <iostream>
#include <sstream>



static std::string
SpeedToString(struct ethtool_cmd* edata)
{
	int speed = ethtool_cmd_speed(edata);
	if (speed == -1)
		return "";
	return int_to_string(speed);
}


static std::string
SpeedToStringWithUnit(struct ethtool_cmd* edata)
{
	int speed = ethtool_cmd_speed(edata);
	if (speed == -1)
		return "";

	std::string unit = "";
	std::string count = "";
	if (speed / 1000 >= 1) {
		unit = "Gb/s";
		count = int_to_string(speed / 1000);
	} else if (speed / 1 >= 1) {
		unit = "Mb/s";
		count = int_to_string(speed / 1);
	}

	if (count == "" && unit == "")
		return "";

	std::string result = "";
	result.append(count).append(" ").append(unit);

#if 0
	// Seems that GLPI doesn't like if we add duplex info here
	std::string duplex = "";
	if (edata->duplex == DUPLEX_FULL)
		duplex = "Full Duplex";
	else if (edata->duplex == DUPLEX_HALF)
		duplex = "Half Duplex";
	
	if (duplex != "")
		result.append(" ").append(duplex);
#endif

	return result;
}


NetworkInterface::NetworkInterface()
{
}


NetworkInterface::NetworkInterface(const char* name)
	:
	fName(name)
{
	if (fName.size() > IFNAMSIZ)
		throw std::runtime_error("NetworkInterface::NetworkInterface(): Name too long");
}


NetworkInterface::~NetworkInterface()
{
}


std::string
NetworkInterface::Name() const
{
	return fName;
}


std::string
NetworkInterface::HardwareAddress() const
{
	return "";
}


bool
NetworkInterface::HasIPAddress() const
{
	std::string ip = IPAddress();
	if (ip.empty() || ip == "" || ip == "0.0.0.0")
		return false;
	return true;
}


std::string
NetworkInterface::IPAddress() const
{
	struct ifreq ifr;
	if (_DoRequest(SIOCGIFADDR, ifr) != 0)
		return "";

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	return inet_ntoa(ipaddr->sin_addr);
}


std::string
NetworkInterface::NetMask() const
{
	return "";
}


std::string
NetworkInterface::Network() const
{
	return "";
}


std::string
NetworkInterface::BroadcastAddress() const
{
	return "";
}


bool
NetworkInterface::HasDefaultGateway() const
{
	std::string gateway = DefaultGateway();
	if (gateway.empty() || gateway == "" || gateway == "0.0.0.0")
		return false;
	return true;
}


std::string
NetworkInterface::DefaultGateway() const
{
	return "";
}


std::string
NetworkInterface::Type() const
{
	// TODO:
	return "Ethernet";
}



std::string
NetworkInterface::Speed() const
{
    return ""; 
}


std::string
NetworkInterface::SpeedWithUnit() const
{
	struct ifreq ifr;
	struct ethtool_cmd edata;

	ifr.ifr_data = (char*)&edata;

	edata.cmd = ETHTOOL_GSET;

	if (_DoRequest(SIOCETHTOOL, ifr) != 0)
		return "0";

    return SpeedToStringWithUnit(&edata);
}


std::string
NetworkInterface::Status() const
{
	return "";
}


bool
NetworkInterface::IsLoopback() const
{
	return false;
}


int
NetworkInterface::_DoRequest(int request, struct ifreq& ifr)  const
{
	int status = 0;

	return status;
}




int
NetworkInterface::_GetRoutes(std::list<route_info>& routeList) const
{
	return 0;	
}
