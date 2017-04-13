/*
 * Machine.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

// TODO: Reorganize code.

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
const char* kMemoryDevice = "Memory Device";

static Machine* sMachine = NULL;


static std::string
GetValueFromMap(std::multimap<std::string, std::string> &map, std::string string, std::string header)
{
	std::map<std::string, std::string>::const_iterator i;
	std::string fullString = header;
	fullString.append(string);
	i = map.find(fullString);
	if (i != map.end())
		return i->second;

	return "";
}


static std::vector<std::string>
GetValuesFromMultiMap(std::multimap<std::string, std::string> &multiMap,
	std::string string, std::string header)
{
	std::vector<std::string> stringList;
	std::pair <std::multimap<std::string, std::string>::const_iterator,
		std::multimap<std::string, std::string>::const_iterator> result;
  
	std::string fullString = header;
	fullString.append(string);
	result = multiMap.equal_range(fullString);
	for (std::multimap<std::string, std::string>::const_iterator i = result.first;
			i != result.second; i++) {
		stringList.push_back(i->second);
	}
	return stringList;
}


/* static */
Machine*
Machine::Get()
{
	if (sMachine == NULL)
		sMachine = new Machine();
	return sMachine;
}


Machine::Machine()
{
	_RetrieveData();
}


Machine::~Machine()
{
}


void
Machine::_RetrieveData()
{
	// Try /sys/devices/virtual/dmi/id tree
	_GetDMIData();
	_GetDMIDecodeData();

	//_GetLSHWData();
	_GetCPUInfo();

	try {
		_GetOSInfo();
	} catch (...) {
		std::cerr << "Failed to get kernel info." << std::endl;
	}
}


std::string
Machine::AssetTag() const
{
	return fChassisInfo.asset_tag;
}


std::string
Machine::BIOSVersion() const
{
	return fBIOSInfo.version;
}


std::string
Machine::BIOSManufacturer() const
{
	return fBIOSInfo.vendor;
}


std::string
Machine::BIOSDate() const
{
	return fBIOSInfo.release_date;
}


std::string
Machine::SystemManufacturer() const
{
	return fSystemInfo.vendor;
}


std::string
Machine::HostName() const
{
	struct ::utsname utsName;
	if (uname(&utsName) != 0)
		return "";

	return utsName.nodename;
}


std::string
Machine::SystemModel() const
{
	return fProductInfo.name;
}


std::string
Machine::SystemSerialNumber() const
{
	return fProductInfo.serial;
}


std::string
Machine::SystemUUID() const
{
	return fProductInfo.uuid;
}


std::string
Machine::MachineSerialNumber() const
{
	return fBoardInfo.serial;
}


std::string
Machine::MachineManufacturer() const
{
	return fBoardInfo.vendor;
}


int
Machine::CountProcessors() const
{
	return fCPUInfo.size();
}


std::string
Machine::ProcessorManufacturer(int numCpu) const
{
	return fCPUInfo[numCpu].manufacturer;
}


std::string
Machine::ProcessorSpeed(int numCpu) const
{
	std::string mhz = fCPUInfo[numCpu].speed;

	size_t pos = mhz.find(".");
	if (pos != std::string::npos) {
		mhz = mhz.substr(0, pos);
	}

	return mhz;
}


std::string
Machine::ProcessorSerialNumber(int numCpu) const
{
	return "";
}


std::string
Machine::ProcessorType(int numCpu) const
{
	std::string model = fCPUInfo[numCpu].type;
	return model;
}


os_info
Machine::OSInfo() const
{
	return fKernelInfo;
}


int
Machine::CountVideos() const
{
	return fVideoInfo.size();
}


video_info
Machine::VideoInfoFor(int numVideo) const
{
	return fVideoInfo[numVideo];
}


// private
bool
Machine::_GetDMIData()
{
	try {
		fBIOSInfo.release_date = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		fBIOSInfo.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		fBIOSInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());
		
		fProductInfo.name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		fProductInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		fProductInfo.uuid = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		fProductInfo.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		
		fChassisInfo.asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		fChassisInfo.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		fChassisInfo.type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
		fChassisInfo.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
		fChassisInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());
		
		fBoardInfo.asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
		fBoardInfo.name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
		fBoardInfo.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
		fBoardInfo.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
		fBoardInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
		
		fSystemInfo.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());
	} catch (...) {
		return false;
	}
	return true;
}


bool
Machine::_GetDMIDecodeData()
{
	if (!CommandExists("dmidecode"))
		return false;

	try {
		popen_streambuf dmi("dmidecode", "r");
		std::istream iStream(&dmi);
		std::string string;
		std::multimap<std::string, std::string> systemInfo;
		while (std::getline(iStream, string)) {
			// Skip the line with "Handle" in it.
			if (string.find("Handle") == std::string::npos) {
				std::string handle = string;
				trim(handle);
				size_t pos = 0;
				while (std::getline(iStream, string)) {
					if (string == "")
						break;

					pos = string.find(":");
					if (pos == std::string::npos)
						continue;

					try {
						std::string name = string.substr(0, pos);
						trim(name);
						std::string fullString = handle;
						fullString.append(name);
						std::string value = string.substr(pos + 2, std::string::npos);

						systemInfo.insert(std::pair<std::string, std::string>(trim(fullString), trim(value)));
					} catch (...) {

					}
				}
			}
		}
		_ExtractNeededInfo(systemInfo);
	} catch (...) {
		return false;
	}

	return true;
}


void
Machine::_ExtractNeededInfo(std::multimap<std::string, std::string> systemInfo)
{
	std::string string;
	string = GetValueFromMap(systemInfo, "Release Date", kBIOSInfo);
	if (string != "" && fBIOSInfo.release_date == "")
		fBIOSInfo.release_date = string;
	string = GetValueFromMap(systemInfo, "Vendor", kBIOSInfo);
	if (string != "" && fBIOSInfo.vendor == "")
		fBIOSInfo.vendor = string;
	string = GetValueFromMap(systemInfo, "Version", kBIOSInfo);
	if (string != "" && fBIOSInfo.version == "")
		fBIOSInfo.version = string;
	string = GetValueFromMap(systemInfo, "Product Name", kSystemInfo);
	if (string != "" && fProductInfo.name == "")
		fProductInfo.name = string;
	string = GetValueFromMap(systemInfo, "Version", kSystemInfo);
	if (string != "" && fProductInfo.version == "")
		fProductInfo.version = string;
	string = GetValueFromMap(systemInfo, "UUID", kSystemInfo);
	if (string != "" && fProductInfo.uuid == "")
		fProductInfo.uuid = string;
	string = GetValueFromMap(systemInfo, "Serial Number", kSystemInfo);
	if (string != "" && fProductInfo.serial == "")
		fProductInfo.serial = string;
	string = GetValueFromMap(systemInfo, "Asset Tag", "Chassis Information");
	if (string != "" && fChassisInfo.asset_tag == "")
		fChassisInfo.asset_tag = string;
	string = GetValueFromMap(systemInfo, "Serial Number", "Chassis Information");
	if (string != "" && fChassisInfo.serial == "")
		fChassisInfo.serial = string;
	string = GetValueFromMap(systemInfo, "Type", "Chassis Information");
	if (string != "" && fChassisInfo.type == "")
		fChassisInfo.type = string;
	string = GetValueFromMap(systemInfo, "Manufacturer", "Chassis Information");
	if (string != "" && fChassisInfo.vendor == "")
		fChassisInfo.vendor = string;
	string = GetValueFromMap(systemInfo, "Version",  "Chassis Information");
	if (string != "" && fChassisInfo.version == "")
		fChassisInfo.version = string;

	string = GetValueFromMap(systemInfo, "Asset Tag", "Base Board Information");
	if (string != "" && fBoardInfo.asset_tag == "")
		fBoardInfo.asset_tag = string;
	string = GetValueFromMap(systemInfo, "Product Name", "Base Board Information");
	if (string != "" && fBoardInfo.name == "")
		fBoardInfo.name = string;
	string = GetValueFromMap(systemInfo, "Manufacturer", "Base Board Information");
	if (string != "" && fBoardInfo.vendor == "")
		fBoardInfo.vendor = string;
	string = GetValueFromMap(systemInfo, "Version", "Base Board Information");
	if (string != "" && fBoardInfo.version == "")
		fBoardInfo.version = string;
	string = GetValueFromMap(systemInfo, "Serial Number", "Base Board Information");
	if (string != "" && fBoardInfo.serial == "")
		fBoardInfo.serial = string;

	string = GetValueFromMap(systemInfo, "Manufacturer", kSystemInfo);
	if (string != "" && fSystemInfo.vendor == "")
		fSystemInfo.vendor = string;


	std::vector<std::string> values = GetValuesFromMultiMap(systemInfo,
											"Size", "Memory Device");
	for (size_t i = 0; i < values.size(); i++) {
		memory_device_info info;
		info.size = values.at(i);
		fMemoryInfo.push_back(info);
	}

	values = GetValuesFromMultiMap(systemInfo, "Bank Locator", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).description = values.at(i);
	values = GetValuesFromMultiMap(systemInfo, "Type", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).type = values.at(i);
	values = GetValuesFromMultiMap(systemInfo, "Speed", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).speed = values.at(i);
	values = GetValuesFromMultiMap(systemInfo, "Manufacturer", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).vendor = values.at(i);
	values = GetValuesFromMultiMap(systemInfo, "Asset Tag", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).asset_tag = values.at(i);
	values = GetValuesFromMultiMap(systemInfo, "Serial Number", "Memory Device");
	for (size_t i = 0; i < values.size(); i++)
		fMemoryInfo.at(i).serial = values.at(i);
}


bool
Machine::_GetLSHWShortData()
{
/*	if (!CommandExists("lshw"))
		return false;

	popen_streambuf lshw("lshw -short", "r");
	std::istream iStream(&lshw);

	try {
		std::string line;

		// header
		std::getline(iStream, line);

		size_t devicePos = line.find("Device");
		size_t classPos = line.find("Class");
		size_t descriptionPos = line.find("Description");

		// skip ==== line
		std::getline(iStream, line);

		while (std::getline(iStream, line)) {
			std::string device = line.substr(devicePos, classPos - devicePos);
			std::string devClass = line.substr(classPos, descriptionPos - classPos);
			std::string value = line.substr(descriptionPos, std::string::npos);
			trim(devClass);
			if (devClass == "system") {
				std::string sysCtx = kSystemInfo;
				sysCtx.append("Product Name");
				if (fSystemInfo.find(sysCtx) == fSystemInfo.end()) {
					fSystemInfo.insert(std::pair<std::string, std::string>(sysCtx, trim(value)));
				}
			} else if (devClass == "display") {
				struct video_info info;
				info.name = trim(value);
				info.chipset = "VGA compatible controller";
				fVideoInfo.push_back(info);
			}

		}
	} catch (...) {

	}
*/
	return true;
}


bool
Machine::_GetLSHWData()
{
/*	if (!CommandExists("lshw"))
		return false;

	popen_streambuf lshw("lshw", "r");
	std::istream iStream(&lshw);

	try {
		std::string line;
		std::string context = kSystemInfo;
		// skip initial line
		std::getline(iStream, line);

		while (std::getline(iStream, line)) {
			trim(line);
			if (size_t start = line.find("*-") != std::string::npos) {
				context = line.substr(start + 2, std::string::npos);
				if (context == "firmware")
					context = kBIOSInfo;
				// TODO: Map other contexts to dmidecode ones
				// TODO: Make this better.
				continue;
			}
			size_t colonPos = line.find(":");
			if (colonPos == std::string::npos)
				continue;

			// TODO: Better mapping of keys
			std::string key = line.substr(0, colonPos);
			trim(key);
			std::string value = line.substr(colonPos + 1, std::string::npos);
			trim(value);
			if (key == "vendor") {
				std::string sysCtx = context;
				sysCtx.append("Manufacturer");
				if (fSystemInfo.find(sysCtx) == fSystemInfo.end()) {
					fSystemInfo.insert(std::pair<std::string, std::string>(sysCtx, trim(value)));
				}
			} else if (key == "product") {
				std::string sysCtx = context;
				sysCtx.append("Product Name");
				if (fSystemInfo.find(sysCtx) == fSystemInfo.end()) {
					fSystemInfo.insert(std::pair<std::string, std::string>(sysCtx, trim(value)));
				}
			}
		}
	} catch (...) {

	}
*/
	return true;
}


void
Machine::_GetCPUInfo()
{
	ProcReader cpu("/proc/cpuinfo");
	std::istream iStream(&cpu);

	std::string string;
	int processorNum = 0;
	while (std::getline(iStream, string)) {
		if (string.find("processor") != std::string::npos) {
			processor_info newInfo;
			fCPUInfo.push_back(newInfo);

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
				name = trim(name);
				
				if (name == "model name")
					fCPUInfo[processorNum].type = trim(value);
				else if (name == "cpu MHz")
					fCPUInfo[processorNum].speed = trim(value);
				else if (name == "vendor_id")
					fCPUInfo[processorNum].manufacturer = trim(value);

			} catch (...) {
			}
		}
	}
}


void
Machine::_GetOSInfo()
{
	struct utsname uName;
	if (::uname(&uName) != 0)
		throw errno;

	fKernelInfo.hostname = uName.nodename;
	fKernelInfo.comments = uName.version;
	fKernelInfo.os_release = uName.release;
	fKernelInfo.domain_name = uName.domainname;
	fKernelInfo.machine = uName.machine;

	ProcReader proc("/proc/meminfo");
	std::istream stream(&proc);

	std::string string;
	size_t pos;
	while (std::getline(stream, string)) {
		if ((pos = string.find("SwapTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string swapString = string.substr(pos + 1, std::string::npos);
			int swapInt = ::strtol(trim(swapString).c_str(), NULL, 10) / 1000;
			fKernelInfo.swap = int_to_string(swapInt);
		} else if ((pos = string.find("MemTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string memString = string.substr(pos + 1, std::string::npos);
			int memInt = ::strtol(trim(memString).c_str(), NULL, 10) / 1000;
			fKernelInfo.memory = int_to_string(memInt);
		}
	}

	fKernelInfo.os_description = _OSDescription();
}


std::string 
Machine::_OSDescription()
{
	std::string osDescription;
	if (CommandExists("lsb_release")) {
		popen_streambuf lsb;
		lsb.open("lsb_release -a", "r");
		std::istream lsbStream(&lsb);
		std::string line;
		while (std::getline(lsbStream, line)) {
			size_t pos = line.find(':');
			if (pos != std::string::npos) {
				std::string key = line.substr(0, pos);
				if (key == "Description") {
					std::string value = line.substr(pos + 1, std::string::npos);
					osDescription = trim(value);
				}
			}
		}
	} else {
		// there is no lsb_release command.
		// try to identify the system in another way
		if (::access("/etc/thinstation.global", F_OK) != -1)
			osDescription = "Thinstation";
		else
			osDescription = "Unknown";
	}
	
	return osDescription;
}


int
Machine::CountMemories()
{
	return fMemoryInfo.size();
}


std::string
Machine::MemoryID(int num)
{
	return int_to_string(num);
}


std::string
Machine::MemoryCaption(int num)
{
	return "";
}


std::string
Machine::MemoryDescription(int num)
{
	return fMemoryInfo.at(num).description;
}


std::string
Machine::MemoryCapacity(int num)
{
	return fMemoryInfo.at(num).size;
}


std::string
Machine::MemoryPurpose(int num)
{
	return fMemoryInfo.at(num).purpose;
}


std::string
Machine::MemoryType(int num)
{
	return fMemoryInfo.at(num).type;
}


std::string
Machine::MemorySpeed(int num)
{
	return fMemoryInfo.at(num).speed;
}


std::string
Machine::MemoryNumSlot(int num)
{
	return int_to_string(num);
}


std::string
Machine::MemorySerialNumber(int num)
{
	return fMemoryInfo.at(num).serial;
}
