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
#include <iostream>
#include <fstream>

#include <assert.h>
#include <unistd.h>


const static char* kServer = "server";
const static char* kDeviceID = "deviceID";
const static char* kOutputFileName = "outputFileName";
const static char* kUseCurrentTimeInDeviceID = "currentTimeInDeviceID";
const static char* kUseBaseBoardSerial = "useBaseBoardSerialNumber";

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


void
Configuration::Print() const
{
	std::cout << "Configuration:" << std::endl;
	try {
		std::cout << "Persistent:" << std::endl;
		std::map<std::string, std::string>::const_iterator i;
		for (i = fValues.begin(); i != fValues.end(); i++) {
			std::cout << i->first << "=" << i->second << std::endl;
		}
		std::cout << "Volatile:" << std::endl;
		for (i = fVolatileValues.begin(); i != fVolatileValues.end(); i++) {
			std::cout << i->first << "=" << i->second << std::endl;
		}
	} catch (...) {
	}
}


void
Configuration::SetServer(const char* serverUrl)
{
	fValues[kServer] = serverUrl;
}


void
Configuration::SetOutputFileName(const char* fileName)
{
	fValues[kOutputFileName] = fileName;
}


void
Configuration::SetKeyValueBoolean(const char* key, bool value)
{
	fValues[key] = _BooleanToString(value);
}


void
Configuration::SetVolatileKeyValueBoolean(const char* key, bool value)
{
	fVolatileValues[key] = _BooleanToString(value);
}


bool
Configuration::KeyValueBoolean(const char* key) const
{
	std::string string = KeyValue(key);
	if (string == "")
		return false;
	return _StringToBoolean(string);
}


void
Configuration::SetKeyValue(const char* key, const char* value)
{
	fValues[key] = value;
}


std::string
Configuration::KeyValue(const char* key) const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(key);
	if (i != fValues.end())
		return i->second;

	// Try volatile values
	i = fVolatileValues.find(key);
	if (i != fVolatileValues.end())
		return i->second;

	return "";
}


void
Configuration::SetVolatileKeyValue(const char* key, const char* value)
{
	fVolatileValues[key] = value;
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


bool
Configuration::UseCurrentTimeInDeviceID() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(kUseCurrentTimeInDeviceID);
	if (i == fValues.end())
		return false;

	return _StringToBoolean(i->second);
}


void
Configuration::SetUseCurrentTimeInDeviceID(bool use)
{
	fValues[kUseCurrentTimeInDeviceID] = _BooleanToString(use);
}


bool
Configuration::UseBaseBoardSerialNumber() const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fValues.find(kUseBaseBoardSerial);
	if (i == fValues.end())
		return false;

	return _StringToBoolean(i->second);
}


void
Configuration::SetUseBaseBoardSerialNumber(bool use)
{
	fValues[kUseBaseBoardSerial] = _BooleanToString(use);
}


void
Configuration::_GenerateDeviceID()
{
	std::string deviceID = Machine::Get()->SystemUUID();
	if (deviceID.length() <= 1) {
		NetworkRoster roster;
		NetworkInterface interface;
		unsigned int cookie = 0;
		while (roster.GetNextInterface(&cookie, interface) == 0) {
			if (!interface.IsLoopback()) {
				deviceID = interface.HardwareAddress();
				deviceID.erase(std::remove(deviceID.begin(), deviceID.end(), ':'),
						deviceID.end());
				break;
			}
		}
	}
	
	if (deviceID == "")
		deviceID = Machine::Get()->HostName();
	
	// DeviceID needs to have a date appended in this very format,
	// otherwise OCSInventoryNG will reject the inventory
	char targetString[256];
	if (UseCurrentTimeInDeviceID()) {
		time_t rawtime = time(NULL);
		struct tm* timeinfo = localtime(&rawtime);
		strftime(targetString, sizeof(targetString), "-%Y-%m-%d-%H-%M-%S", timeinfo);
	} else {
		std::string biosDateString = Machine::Get()->BIOSDate();
		// On some machines, this can be empty. So use an harcoded
		// value, since we need a correct date for the device id
		if (biosDateString.length() <= 1)
			biosDateString = "01/01/2017";
		struct tm biosDate;
		strptime(biosDateString.c_str(), "%m/%d/%Y", &biosDate);
		strftime(targetString, sizeof(targetString), "-%Y-%m-%d-00-00-00", &biosDate);
	}

    deviceID.append(targetString);

	fValues[kDeviceID] = deviceID;
}


std::string
Configuration::_BooleanToString(bool value)
{
	return std::string(value ? "true" : "false");
}


bool
Configuration::_StringToBoolean(const std::string string)
{
	std::string lowerCaseString = string;
	std::transform(lowerCaseString.begin(), lowerCaseString.end(),
		lowerCaseString.begin(), ::tolower);
	if (lowerCaseString.compare("yes") == 0
		|| lowerCaseString.compare("true") == 0)
		return true;
	return false;
}
