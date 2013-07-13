/*
 * Machine.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Machine.h"
#include "Support.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

Machine::Machine()
	:
	fNumCPUs(0)
{
}


Machine::~Machine()
{
}


void
Machine::RetrieveData()
{
	try {
		// TODO: If dmidecode isn't installed, read from /proc ?
		_GetDMIDecodeData();
	} catch (...) {
		std::cerr << "Can't find dmidecode. Is it installed?" << std::endl;
	}

	_GetCPUInfo();
}


std::string
Machine::BIOSVersion() const
{
	return _GetValue("Version", "BIOS Information");
}


std::string
Machine::BIOSManufacturer() const
{
	return _GetValue("Vendor", "BIOS Information");
}


std::string
Machine::BIOSDate() const
{
	return _GetValue("Release Date", "BIOS Information");
}


std::string
Machine::MachineManufacturer() const
{
	return _GetValue("Manufacturer", "System Information");

}


std::string
Machine::SystemModel() const
{
	return _GetValue("Product Name", "System Information");
}


std::string
Machine::SystemSerialNumber() const
{
	return _GetValue("Serial Number", "System Information");
}


int
Machine::CountProcessors() const
{
	return fNumCPUs;
}


std::string
Machine::ProcessorInfo(const char* info, int num) const
{
	if (num < 0 || num >= fNumCPUs)
		return "";

	std::map<std::string, std::string>::const_iterator i;
	i = fCPUInfo[num].find(info);
	if (i != fCPUInfo[num].end())
		return i->second;
	return "";
}


void
Machine::_GetDMIDecodeData()
{
	popen_streambuf dmi("dmidecode", "r");
	std::istream iStream(&dmi);

	std::string string;
	while (std::getline(iStream, string) > 0) {
		// Skip the line with "Handle" in it.
		if (string.find("Handle") == std::string::npos) {
			std::string header = string;
			header = trim(header);

			_GetSystemInfo(iStream, header);
		}
	}
}


void
Machine::_GetCPUInfo()
{
	popen_streambuf cpu("cat /proc/cpuinfo", "r");
	std::istream iStream(&cpu);

	std::string string;
	int processorNum = 0;
	while (std::getline(iStream, string) > 0) {
		if (string.find("processor") != std::string::npos) {
			fNumCPUs++;
			// Get the processor number
			size_t pos = string.find(":");
			if (pos == std::string::npos)
				continue;

			std::string valueString = string.substr(pos + 2, std::string::npos);
			trim(valueString);
			processorNum = ::strtol(valueString.c_str(), NULL, 10);
			std::cout << "Processor number " << valueString << std::endl;

		} else {
			size_t pos = string.find(":");
			if (pos == std::string::npos)
				continue;

			try {
				std::string name = string.substr(0, pos);
				std::string value = string.substr(pos + 1, std::string::npos);

				fCPUInfo[processorNum][trim(name)] = trim(value);

			} catch (...) {
			}
		}
	}
}


void
Machine::_GetSystemInfo(std::istream& stream, std::string header)
{
	//std::cout << header << std::endl;
	std::string string;
	size_t pos = 0;
	while (std::getline(stream, string) > 0) {
		//std::cout << "*** " << string << "*** " << std::endl;
		if (string == "")
			break;

		pos = string.find(":");
		if (pos == std::string::npos)
			continue;

		try {
			std::string name = string.substr(0, pos);
			trim(name);
			// TODO: We should prepend the header, actually
			name.append(header);
			std::string value = string.substr(pos + 2, std::string::npos);

			fSystemInfo[trim(name)] = trim(value);

		} catch (...) {

		}
	}
}


std::string
Machine::_GetValue(std::string string, std::string header) const
{
	std::map<std::string, std::string>::const_iterator i;
	i = fSystemInfo.find(string.append(header));
	if (i != fSystemInfo.end())
		return i->second;

	return "";
}
