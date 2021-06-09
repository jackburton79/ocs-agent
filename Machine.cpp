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
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <tinyxml2/tinyxml2.h>

#include <XML.h>

typedef std::map<std::string, std::string> string_map;

const char* kMemoryDevice = "Memory Device";

static Machine* sMachine = NULL;


std::map<std::string, Component> gComponents;


void
Component::MergeWith(Component& component)
{
	fields.insert(component.fields.begin(), component.fields.end());
};


// Returns size, in MBytes,
// starting from a string like '3GB' or '1024 KB'
unsigned int
convert_to_MBytes(const std::string& string)
{
	char *memoryUnit = NULL;
	unsigned int memorySize = ::strtol(string.c_str(), &memoryUnit, 10);
	std::string unit = memoryUnit;
	trim(unit);
	if (::strcasecmp(unit.c_str(), "KB") == 0
		|| ::strcasecmp(unit.c_str(), "KiB") == 0)
		memorySize /= 1024;
	else if (::strcasecmp(unit.c_str(), "GB") == 0
		 || ::strcasecmp(unit.c_str(), "GiB") == 0)
		memorySize *= 1024;
	return memorySize;
}





// OSInfoBackend
OSInfoBackend::OSInfoBackend()
{
}


/* virtual */
OSInfoBackend::~OSInfoBackend()
{
}


/* virtual */
int
OSInfoBackend::Run()
{
	std::string line;
	std::string osDescription;
	Component os;
	if (::access("/etc/os-release", F_OK) != -1) {
		ProcReader osReader("/etc/os-release");
		try {
			while ((line = osReader.ReadLine()) != "") {
				if (line.find("PRETTY_NAME") != std::string::npos) {
					size_t pos = line.find('=');
					if (pos != std::string::npos) {
						std::string value = line.substr(pos + 1, std::string::npos);
						value = trim(value);
						// remove quotes
						osDescription = value.substr(1, value.length() - 2);
						break;
					}
				}
			}
		} catch (...) {
			// not an error
		}
	} else if (CommandExists("lsb_release")) {
		CommandStreamBuffer lsb;
		lsb.open("lsb_release -a", "r");
		std::istream lsbStream(&lsb);
		while (std::getline(lsbStream, line)) {
			if (line.find("Description") != std::string::npos) {
				size_t pos = line.find(':');
				if (pos != std::string::npos) {
					std::string value = line.substr(pos + 1, std::string::npos);
					osDescription = trim(value);
					break;
				}
			}
		}
	} else if (::access("/etc/thinstation.global", F_OK) != -1) {
		osDescription = "Thinstation";
		char* tsVersion = ::getenv("TS_VERSION");
		if (tsVersion != NULL) {
			osDescription += " ";
			osDescription += tsVersion;
		}
	} else {
		try {
			osDescription = trimmed(ProcReader("/etc/redhat-release").ReadLine());
		} catch (...) {
			osDescription = "Unknown";
		}
	}

	os.fields["description"] = osDescription;
	gComponents["OS"].MergeWith(os);
	return 0;
}


// Machine
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
}


Machine::~Machine()
{
}


int
Machine::CountMemories()
{
	return fMemoryInfo.size();
}


std::string
Machine::MemoryCaption(int num)
{
	return fMemoryInfo.at(num).caption;
}


std::string
Machine::MemoryDescription(int num)
{
	return fMemoryInfo.at(num).description;
}


std::string
Machine::MemoryCapacity(int num)
{
	return fMemoryInfo.at(num).Size();
}


std::string
Machine::MemoryPurpose(int num)
{
	return fMemoryInfo.at(num).purpose;
}


std::string
Machine::MemoryType(int num)
{
	return fMemoryInfo.at(num).Type();
}


std::string
Machine::MemorySpeed(int num)
{
	return fMemoryInfo.at(num).Speed();
}


std::string
Machine::MemoryNumSlot(int num)
{
	return int_to_string(num + 1);
}


std::string
Machine::MemorySerialNumber(int num)
{
	return fMemoryInfo.at(num).serial;
}



/*
void
Machine::_ExtractDataFromDMIDB(dmi_db dmiDb)
{
	bios_info biosInfo;
	biosInfo.release_date = GetValueFromMap(dmiDb, "Release Date", kBIOSInfo);
	biosInfo.vendor = GetValueFromMap(dmiDb, "Vendor", kBIOSInfo);
	biosInfo.version = GetValueFromMap(dmiDb, "Version", kBIOSInfo);
	fBIOSInfo.MergeWith(biosInfo);

	system_info systemInfo;
	systemInfo.name = GetValueFromMap(dmiDb, "Product Name", kSystemInfo);
	systemInfo.version = GetValueFromMap(dmiDb, "Version", kSystemInfo);
	systemInfo.uuid = GetValueFromMap(dmiDb, "UUID", kSystemInfo);
	systemInfo.serial = GetValueFromMap(dmiDb, "Serial Number", kSystemInfo);
	systemInfo.vendor = GetValueFromMap(dmiDb, "Manufacturer", kSystemInfo);
	fSystemInfo.MergeWith(systemInfo);

	chassis_info chassisInfo;
	chassisInfo.asset_tag = GetValueFromMap(dmiDb, "Asset Tag", "Chassis Information");
	chassisInfo.serial = GetValueFromMap(dmiDb, "Serial Number", "Chassis Information");
	chassisInfo.type = GetValueFromMap(dmiDb, "Type", "Chassis Information");
	chassisInfo.vendor = GetValueFromMap(dmiDb, "Manufacturer", "Chassis Information");
	chassisInfo.version = GetValueFromMap(dmiDb, "Version",  "Chassis Information");
	fChassisInfo.MergeWith(chassisInfo);

	board_info boardInfo;
	boardInfo.asset_tag = GetValueFromMap(dmiDb, "Asset Tag", "Base Board Information");
	boardInfo.name = GetValueFromMap(dmiDb, "Product Name", "Base Board Information");
	boardInfo.vendor = GetValueFromMap(dmiDb, "Manufacturer", "Base Board Information");
	boardInfo.version = GetValueFromMap(dmiDb, "Version", "Base Board Information");
	boardInfo.serial = GetValueFromMap(dmiDb, "Serial Number", "Base Board Information");
	fBoardInfo.MergeWith(boardInfo);

	std::vector<string_map> valuesVector;
	DMIExtractor dmiExtractor(dmiDb);
	std::vector<string_map>::iterator i;

	// Graphics cards
	if (fVideoInfo.size() == 0) {
		valuesVector = dmiExtractor.ExtractEntry("Display");
		for (i = valuesVector.begin(); i != valuesVector.end(); i++) {
			string_map& entry = *i;
			video_info info;
			try {
				string_map::const_iterator mapIter;
				mapIter = entry.find("Manufacturer");
				if (mapIter != entry.end())
					info.vendor = mapIter->second;
				mapIter = entry.find("Product Name");
				if (mapIter != entry.end())
					info.name = mapIter->second;
				mapIter = entry.find("description");
				if (mapIter != entry.end())
					info.chipset = mapIter->second;
				fVideoInfo.push_back(info);
			} catch (...) {
			}
		}
	}

	// Memory slots
	if (fMemoryInfo.size() > 0)
		return;

	valuesVector = dmiExtractor.ExtractEntry(kMemoryDevice);
	for (i = valuesVector.begin(); i != valuesVector.end(); i++) {
		string_map& entry = *i;
		memory_device_info info;
		try {
			string_map::const_iterator mapIter;
			mapIter = entry.find("Size");
			if (mapIter != entry.end()) {
				info.size = convert_to_MBytes(mapIter->second);
			} else
				info.size = 0;

			mapIter = entry.find("Locator");
			if (mapIter != entry.end())
				info.description = mapIter->second;

			mapIter = entry.find("Type");
			if (mapIter != entry.end())
				info.type = mapIter->second;

			mapIter = entry.find("Speed");
			if (mapIter != entry.end())
				info.speed = ::strtoul(mapIter->second.c_str(), NULL, 10);

			mapIter = entry.find("Manufacturer");
			if (mapIter != entry.end())
				info.vendor = mapIter->second;
			mapIter = entry.find("Asset Tag");
			if (mapIter != entry.end())
				info.asset_tag = mapIter->second;
			mapIter = entry.find("Serial Number");
			if (mapIter != entry.end())
				info.serial = mapIter->second;

			mapIter = entry.find("Array Handle");
			if (mapIter != entry.end()) {
				std::string parentHandle = mapIter->second;
				string_map arrayHandle = dmiExtractor.ExtractHandle(parentHandle);
				mapIter = arrayHandle.find("Use");
				if (mapIter != arrayHandle.end())
					info.purpose = mapIter->second;
				mapIter = arrayHandle.find("Use");
				if (mapIter != arrayHandle.end())
					info.caption = mapIter->second;
			}
		} catch (...) {
		}

		// Make sure we have at least some valid info
		if (info.caption != "" || info.purpose != ""
			|| info.type != "" || info.serial != "" || info.speed != 0)
			fMemoryInfo.push_back(info);
	}
}
*/


// memory_device_info
memory_device_info::memory_device_info()
	:
	speed(0),
	size(0)
{
}


std::string
memory_device_info::Type() const
{
	if (size == 0)
		return "Empty slot";
	return type;
}


std::string
memory_device_info::Speed() const
{
	return int_to_string(speed);
}


std::string
memory_device_info::Size() const
{
	return int_to_string(size);
}
