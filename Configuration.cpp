/*
 * Configuration.cpp
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#include "Configuration.h"

#include <cerrno>
#include <fstream>

#include <assert.h>
#include <unistd.h>

#include <iostream>
#include <ctime>


const static char* kServer = "server";
const static char* kDeviceID = "deviceID";


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
	fConfigFileName = fileName;
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

	// Generate a DeviceID if we don't have one
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(kDeviceID);
	if (i == fValues.end())
		_GenerateDeviceID();

	return true;
}


bool
Configuration::Save(const char* fileName)
{
	try {
		std::ofstream configFile(fileName, std::ios_base::out);
		std::map<std::string, std::string>::const_iterator i;
		for (i = fValues.begin(); i != fValues.end(); i++) {
			configFile << i->first << "=" << i->second << std::endl;
		}
	} catch (...) {
		return false;
	}
	return true;
}


bool
Configuration::Save()
{
	if (fConfigFileName == "")
		return false;

	return Save(fConfigFileName.c_str());
}


bool
Configuration::SetServer(const char* serverUrl)
{
	fValues[kServer] = serverUrl;
	return true;
}


std::string
Configuration::DeviceID() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(kDeviceID);
	if (i == fValues.end()) {
		const_cast<Configuration*>(this)->_GenerateDeviceID();
		i = fValues.find(kDeviceID);
	}

	assert(i->second != "");
	return i->second;
}


std::string
Configuration::ServerURL() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find("server");
	if (i != fValues.end())
		return i->second;

	return "";
}


bool
Configuration::LocalInventory() const
{
	return fValues.find(kServer) == fValues.end();
}


void
Configuration::_GenerateDeviceID()
{
	char buffer[64];
	if (::gethostname(buffer, sizeof(buffer)) != 0)
		throw errno;

	std::string deviceID;
	deviceID.append(buffer);
	// TODO: PXE-booted machines can't have this saved anywhere.
	// Either use the MAC address to generate this, or just keep it fixed.
	
	time_t rawtime;
  	struct tm * timeinfo;
  	char buffer2[80];

  	time (&rawtime);
  	timeinfo = localtime(&rawtime);

	strftime(buffer2,80,"%Y-%m-%d %H:%M:%S",timeinfo);
	
	std::string str(buffer2);

	//  std::cout << str;
        deviceID.append(str);
	//deviceID.append("-2013-05-10-10-10-10");

	fValues[kDeviceID] = deviceID;
}
