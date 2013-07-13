/*
 * Machine.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Machine.h"
#include "Support.h"

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <map>
#include <string>

Machine::Machine()
{
}


Machine::~Machine()
{
}


void
Machine::RetrieveData()
{
	// TODO: If dmidecode isn't installed, read from /proc ?
	_GetDMIDecodeData();
}


std::string
Machine::BIOSVersion() const
{
	return _GetBIOSValue("Version");
}


std::string
Machine::BIOSManufacturer() const
{
	return _GetBIOSValue("Vendor");
}


std::string
Machine::BIOSDate() const
{
	return _GetBIOSValue("Release Date");
}


std::string
Machine::MachineManufacturer() const
{
	return _GetSystemValue("Manufacturer");

}


std::string
Machine::SystemModel() const
{
	return _GetSystemValue("Product Name");
}


std::string
Machine::SystemSerialNumber() const
{
	return _GetSystemValue("Serial Number");
}


void
Machine::_GetDMIDecodeData()
{
	popen_streambuf dmi("dmidecode", "r");
	std::istream iStream(&dmi);

	std::string string;
	while (std::getline(iStream, string) > 0) {
		//std::cout << "*** " << string << " ***" << std::endl;
		if (string.find("BIOS Information") != std::string::npos)
			_GetBIOSInfo(iStream);
		else if (string.find("System Information") != std::string::npos) {
			_GetSystemInfo(iStream);
		}
	}
}


void
Machine::_GetBIOSInfo(std::istream& stream)
{
	std::cout << "GetBIOSInfo()" << std::endl;

	std::string string;
	size_t pos = 0;
	while (std::getline(stream, string) > 0) {
		std::cout << "bios ** " << string << "*** " << std::endl;
		if (string == "")
			break;

		pos = string.find(":");
		if (pos == std::string::npos)
			continue;

		try {
			std::string name = string.substr(0, pos);
			std::string value = string.substr(pos + 2, std::string::npos);

			fBIOSInfo[trim(name)] = trim(value);
		} catch (...) {
			// TODO: Handle exceptions better (i.e. out_of_range)
		}
	}
}


void
Machine::_GetSystemInfo(std::istream& stream)
{
	std::cout << "GetSystemInfo()" << std::endl;

	std::string string;
	size_t pos = 0;
	while (std::getline(stream, string) > 0) {
		std::cout << "sys ** " << string << "*** " << std::endl;
		if (string == "")
			break;

		pos = string.find(":");
		if (pos == std::string::npos)
			continue;

		try {
			std::string name = string.substr(0, pos);
			std::string value = string.substr(pos + 2, std::string::npos);

			fSystemInfo[trim(name)] = trim(value);

		} catch (...) {

		}
	}
}


std::string
Machine::_GetBIOSValue(std::string string) const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fBIOSInfo.find(string);
	if (i != fBIOSInfo.end())
		return i->second;

	return "";
}


std::string
Machine::_GetSystemValue(std::string string) const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fSystemInfo.find(string);
	if (i != fSystemInfo.end())
		return i->second;

	return "";
}
