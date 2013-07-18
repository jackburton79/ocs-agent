/*
 * Machine.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

// TODO: Reorganize code.

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

const char* kBIOSInfo = "BIOS Information";
const char* kSystemInfo = "System Information";
const char* kProcessorInfo = "Processor Info";

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

	try {
		_GetKernelInfo();
	} catch (...) {
		std::cerr << "Failed to get kernel info." << std::endl;
	}
}


std::string
Machine::AssetTag() const
{
	return "";
}


std::string
Machine::BIOSVersion() const
{
	return _GetValue("Version", kBIOSInfo);
}


std::string
Machine::BIOSManufacturer() const
{
	return _GetValue("Vendor", kBIOSInfo);
}


std::string
Machine::BIOSDate() const
{
	return _GetValue("Release Date", kBIOSInfo);
}


std::string
Machine::MachineManufacturer() const
{
	return _GetValue("Manufacturer", kSystemInfo);

}


std::string
Machine::SystemModel() const
{
	return _GetValue("Product Name", kSystemInfo);
}


std::string
Machine::SystemSerialNumber() const
{
	return _GetValue("Serial Number", kSystemInfo);
}


int
Machine::CountProcessors() const
{
	return fNumCPUs;
}


std::string
Machine::ProcessorManufacturer(int numCpu) const
{
	return _ProcessorInfo("vendor_id", numCpu);
}


std::string
Machine::ProcessorSpeed(int numCpu) const
{
	std::string mhz = _ProcessorInfo("cpu MHz", numCpu);

	size_t pos = mhz.find(".");
	if (pos != std::string::npos) {
		mhz = mhz.substr(0, pos);
	}

	return mhz;
}


std::string
Machine::ProcessorSerialNumber(int numCpu) const
{
	//std::string mhz = _GetValue("Serial Number", numCpu);
	return "";
}


std::string
Machine::ProcessorType(int numCpu) const
{
	std::string model = _ProcessorInfo("model name", numCpu);
	trim(model);
	return model;
}


kernel_info
Machine::KernelInfo() const
{
	return fKernelInfo;
}



// private
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
	// TODO: Use ProcReader when it's ready for this
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
Machine::_GetKernelInfo()
{
	struct utsname uName;
	if (::uname(&uName) != 0)
		throw errno;

	//std::cout << uName.sysname << std::endl;
	fKernelInfo.hostname = uName.nodename;
	fKernelInfo.comments = uName.version;
	fKernelInfo.os_release = uName.release;
	//std::cout << uName.machine << std::endl;
	fKernelInfo.domain_name = uName.domainname;

	ProcReader proc("meminfo");
	std::istream stream(&proc);

	std::string string;
	size_t pos;
	while (std::getline(stream, string) > 0) {
		if ((pos = string.find("SwapTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string swapString = string.substr(pos + 1, std::string::npos);

			int swapInt = ::strtol(trim(swapString).c_str(), NULL, 10) / 1024;
			std::ostringstream stringStream;
			stringStream << swapInt;
			fKernelInfo.swap = stringStream.str();
		} else if ((pos = string.find("MemTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string memString = string.substr(pos + 1, std::string::npos);
			int memInt = ::strtol(trim(memString).c_str(), NULL, 10) / 1024;
			std::ostringstream stringStream;
			stringStream << memInt;
			fKernelInfo.memory = stringStream.str();
		}
	}
	//fKernelInfo.memory =
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



std::string
Machine::_ProcessorInfo(const char* info, int num) const
{
	if (num < 0 || num >= fNumCPUs)
		return "";

	std::map<std::string, std::string>::const_iterator i;
	i = fCPUInfo[num].find(info);
	if (i != fCPUInfo[num].end())
		return i->second;
	return "";
}
