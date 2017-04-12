/*
 * Configuration.cpp
 *
 *  Created on: 13/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Configuration.h"
#include "Machine.h"
#include "NetworkInterface.h"
#include "NetworkRoster.h"

#include <algorithm>
#include <cerrno>
#include <ctime>
#include <fstream>

#include <assert.h>
#include <unistd.h>


const static char* kServer = "server";
const static char* kDeviceID = "deviceID";
const static char* kOutputFileName = "outputFileName";


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
		while (std::getline(configFile, line)) {
			size_t pos = line.find("=");
			if (pos != std::string::npos) {
				fValues[line.substr(0, pos)] = line.substr(pos + 1, std::string::npos);
			}
		}
	} catch (...) {
	}

	// Generate a DeviceID if we don't have one
	if (fValues.find(kDeviceID) == fValues.end())
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


bool
Configuration::SetOutputFileName(const char* fileName)
{
	fValues[kOutputFileName] = fileName;
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
	i = fValues.find(kServer);
	if (i != fValues.end())
		return i->second;

	return "";
}


bool
Configuration::LocalInventory() const
{
	return fValues.find(kServer) == fValues.end();
}


std::string
Configuration::OutputFileName() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(kOutputFileName);
	if (i != fValues.end())
		return i->second;
	return "";
}


void
Configuration::_GenerateDeviceID()
{
	std::string deviceID = Machine::Get()->SystemUUID();
	if (deviceID.length() <= 1 || ) {
		NetworkRoster roster;
		NetworkInterface interface;
		unsigned int cookie = 0;
		while (roster.GetNextInterface(&cookie, interface) == 0) {
			if (interface.Name() != "lo") {
				deviceID = interface.HardwareAddress();
				deviceID.erase(std::remove(deviceID.begin(), deviceID.end(), ':'),
						deviceID.end());
				break;
			}
		}
	}
	
	if (deviceID == "")
		deviceID = Machine::Get()->HostName();
	
	std::string biosDateString = Machine::Get()->BIOSDate();
	struct tm biosDate;
	strptime(biosDateString.c_str(), "%m/%d/%Y", &biosDate);
	
	// DeviceID needs to have a date appended in this very format,
	// otherwise OCSInventoryNG will reject the inventory
	char targetString[128];
	strftime(targetString, sizeof(targetString), "-%Y-%m-%d-00-00-00", &biosDate);
    deviceID.append(targetString);

	fValues[kDeviceID] = deviceID;
}
