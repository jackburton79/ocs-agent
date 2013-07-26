/*
 * Configuration.cpp
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#include "Configuration.h"

#include <cerrno>
#include <fstream>

#include <unistd.h>


static Configuration* sConfiguration;

Configuration::Configuration()
{
}


Configuration::~Configuration()
{
}


/* static */
Configuration*
Configuration::Get()
{
	if (sConfiguration == NULL)
		sConfiguration = new Configuration;
	return sConfiguration;
}


bool
Configuration::Load(const char* fileName)
{
	try {
		std::ifstream configFile(fileName);
		std::string line;
		while (std::getline(configFile, line) > 0) {
			size_t pos = line.find("=");
			if (pos != std::string::npos) {
				fValues[line.substr(0, pos)] = line.substr(pos + 1, std::string::npos);
			}
		}
	} catch (...) {
	}
	return true;
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
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find("server");
	if (i != fValues.end())
		return i->second;

	return "http://ocsinventory-ng/ocsinventory";
}


bool
Configuration::LocalInventory() const
{
	return false;
}

