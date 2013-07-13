/*
 * Configuration.cpp
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#include "Configuration.h"

#include <errno.h>
#include <unistd.h>


Configuration::Configuration()
{
}


Configuration::~Configuration()
{
}


std::string
Configuration::DeviceID() const
{
	std::string hostName;
	char buffer[64];
	if (gethostname(buffer, sizeof(buffer)) != 0)
		throw errno;

	hostName.append(buffer);
	// TODO: PXE-booted machines can't have this saved anywhere.
	// Either use the MAC address to generate this, or just keep it fixed.
	hostName.append("-2013-05-10-10-10-10");
	return hostName;
}


std::string
Configuration::ServerURL() const
{
	// TODO: Make this non-fixed.
	return "http://ocsinventory-ng";
}


bool
Configuration::LocalInventory() const
{
	return true;
}

