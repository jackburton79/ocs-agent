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
#include <sys/utsname.h>

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

const char* kBIOSInfo = "BIOS Information";
const char* kSystemInfo = "System Information";
const char* kProcessorInfo = "Processor Info";
const char* kMemoryDevice = "Memory Device";

static Machine* sMachine = NULL;


std::map<std::string, Component> gComponents;


void
Component::MergeWith(Component& component)
{
	if (asset_tag.empty())
		asset_tag = component.asset_tag;
	if (serial.empty())
		serial = component.serial;
	if (vendor.empty())
		vendor = component.vendor;
	if (release_date.empty())
		release_date = component.release_date;
	if (version.empty())
		version = component.version;
	if (type.empty())
		type = component.type;

};


/*
static std::string
GetValueFromMap(dmi_db &db, std::string key, std::string context)
{
	DMIExtractor extractor(db);
	if (extractor.CountEntries(context) <= 0)
		return "";

	std::vector<string_map> entryVector
		= extractor.ExtractEntry(context);

	string_map &map = entryVector[0];
	string_map::const_iterator i;
	i = map.find(key);
	if (i != map.end())
		return i->second;

	return "";
}
*/

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


std::string
RAM_type_from_description(const std::string& description)
{
	std::string type;
	if (!description.empty()) {
		// TODO: Not the cleanest approach, but lshw doesn't
		// seem to return this in any other field
		if (description.find("SDRAM") != std::string::npos)
			type = "SDRAM";
		else if (description.find("FLASH") != std::string::npos)
			type = "FLASH";
		else if (description.find("DDR") != std::string::npos)
			type = "DDR";
		// TODO: Yeah, and DDR2 ? DDR3 ?
		// TODO: Handle empty slots like we do for dmidecode
	}
	return type;
}


// OSInfo
OSInfo::OSInfo()
{
	struct utsname uName;
	if (::uname(&uName) != 0) {
		std::string errorString = "OSInfo::_GetOsInfo(): uname() failed with error ";
		errorString.append(::strerror(errno));
		throw std::runtime_error(errorString);
	}

	hostname = uName.nodename;
	comments = uName.version;
	release = uName.release;
	domainname = uName.domainname;
	architecture = uName.machine;

	//Feed domain name from host name when possible.
	if (domainname == "" || domainname == "(none)") {
		size_t dotPos = hostname.find('.');
		if (dotPos != std::string::npos) {
			domainname = hostname.substr(dotPos + 1);
		}
	}

	ProcReader proc("/proc/meminfo");
	std::istream stream(&proc);

	std::string string;
	while (std::getline(stream, string)) {
		size_t pos;
		if ((pos = string.find("SwapTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string swapString = string.substr(pos + 1, std::string::npos);
			int swapInt = ::strtol(trim(swapString).c_str(), NULL, 10) / 1000;
			swap = int_to_string(swapInt);
		} else if ((pos = string.find("MemTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string memString = string.substr(pos + 1, std::string::npos);
			int memInt = ::strtol(trim(memString).c_str(), NULL, 10) / 1000;
			memory = int_to_string(memInt);
		}
	}

	description = _OSDescription();
}


std::string
OSInfo::_OSDescription()
{
	std::string line;
	std::string osDescription;
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

	return osDescription;
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
	_RetrieveData();
}


Machine::~Machine()
{
}


void
Machine::_RetrieveData()
{
	try {
		// Try /sys/devices/virtual/dmi/id tree, then 'dmidecode', then 'lshw'
		_GetDMIData();
		//_GetGraphicsCardInfo();
		//_GetDMIDecodeData();
		_GetLSHWData();
	} catch (...) {
		std::cerr << "Failed to get hardware info." << std::endl;
	}
/*
	components_map::const_iterator i;
	for (i = gComponents.begin(); i != gComponents.end(); i++) {
		std::cout << (*i).second.name << std::endl;
		std::cout << (*i).second.release_date << std::endl;
		std::cout << (*i).second.vendor << std::endl;
		std::cout << (*i).second.serial << std::endl;
		std::cout << (*i).second.version << std::endl;
		std::cout << (*i).second.type << std::endl;
	}*/
}


std::string
Machine::AssetTag() const
{
	return gComponents["CHASSIS"].asset_tag;
}


std::string
Machine::BIOSVersion() const
{
	return gComponents["BIOS"].version;
}


std::string
Machine::BIOSManufacturer() const
{
	return gComponents["BIOS"].vendor;
}


std::string
Machine::BIOSDate() const
{
	return gComponents["BIOS"].release_date;
}


std::string
Machine::SystemManufacturer() const
{
	return gComponents["SYSTEM"].vendor;
}


std::string
Machine::SystemModel() const
{
	return gComponents["SYSTEM"].name;
}


std::string
Machine::SystemSerialNumber() const
{
	// Some systems have this empty, or, like our MCP79s,
	// "To Be Filled by O.E.M.", which is pretty much useless,
	// so in that case we use the baseboard serial number
	if (gComponents["SYSTEM"].serial.empty()
		|| gComponents["SYSTEM"].serial == "To Be Filled By O.E.M.")
		return MachineSerialNumber();

	return gComponents["SYSTEM"].serial;
}


std::string
Machine::SystemUUID() const
{
	return gComponents["SYSTEM"].uuid;
}


std::string
Machine::SystemType() const
{
	return gComponents["CHASSIS"].type;
}


std::string
Machine::MachineSerialNumber() const
{
	return gComponents["BOARD"].serial;
}


std::string
Machine::MachineManufacturer() const
{
	return gComponents["BOARD"].vendor;
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


// private
bool
Machine::_GetDMIData()
{
	// DMIDataBackend
	try {
		gComponents["BIOS"].release_date = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		gComponents["BIOS"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		gComponents["BIOS"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());

		gComponents["SYSTEM"].name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		gComponents["SYSTEM"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		gComponents["SYSTEM"].uuid = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		gComponents["SYSTEM"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		gComponents["SYSTEM"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/sys_vendor").ReadLine());

		gComponents["CHASSIS"].asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		gComponents["CHASSIS"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		//gComponents["CHASSIS"].type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
		gComponents["CHASSIS"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_vendor").ReadLine());
		gComponents["CHASSIS"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_version").ReadLine());

		gComponents["BOARD"].asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_asset_tag").ReadLine());
		gComponents["BOARD"].name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_name").ReadLine());
		gComponents["BOARD"].serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_serial").ReadLine());
		gComponents["BOARD"].vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_vendor").ReadLine());
		gComponents["BOARD"].version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/board_version").ReadLine());
	} catch (...) {
		return false;
	}
	return true;
}


bool
Machine::_GetGraphicsCardInfo()
{
	// TODO: Does not work with multiple video cards. And does not work well in general
	struct video_info videoInfo;
	try {
		videoInfo.name = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_product_name").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.vendor = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_vendor").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.chipset = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_string").ReadLine());
	} catch (...) {
	}
	try {
		videoInfo.resolution = trimmed(ProcReader("/sys/class/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	// try this other path
	try {
		videoInfo.resolution = trimmed(ProcReader("/sys/class/graphics/fb0/device/graphics/fb0/virtual_size").ReadLine());
	} catch (...) {
	}
	std::replace(videoInfo.resolution.begin(), videoInfo.resolution.end(), ',', 'x');
	if (videoInfo.resolution.empty() && videoInfo.name.empty() && videoInfo.chipset.empty())
		return false;

	fVideoInfo.push_back(videoInfo);
	return true;
}




bool
Machine::_GetLSHWData()
{
	if (!CommandExists("lshw"))
		return false;

	// Load command output into "string"
	CommandStreamBuffer lshw("lshw -xml", "r");
	std::istream iStream(&lshw);
	std::istreambuf_iterator<char> eos;
	std::string string(std::istreambuf_iterator<char>(iStream), eos);

	tinyxml2::XMLDocument doc;
	if (doc.Parse(string.c_str(), string.size()) != tinyxml2::XML_SUCCESS)
		return false;

	const tinyxml2::XMLElement* element = NULL;
	const tinyxml2::XMLElement* tmpElement = NULL;
	element = XML::GetElementByAttribute(doc, "id", "firmware");
	if (element != NULL) {
		Component biosInfo;
		biosInfo.release_date = XML::GetFirstChildElementText(element, "date");
		biosInfo.vendor = XML::GetFirstChildElementText(element, "vendor");
		biosInfo.version = XML::GetFirstChildElementText(element, "version");
		gComponents["BIOS"].MergeWith(biosInfo);
	}

	element = XML::GetElementByAttribute(doc, "class", "system");
	if (element != NULL) {
		Component systemInfo;
		systemInfo.name = XML::GetFirstChildElementText(element, "product");
		systemInfo.version = XML::GetFirstChildElementText(element, "version");
		systemInfo.serial = XML::GetFirstChildElementText(element, "serial");
		systemInfo.vendor = XML::GetFirstChildElementText(element, "vendor");
		gComponents["SYSTEM"].MergeWith(systemInfo);

		Component chassisInfo;
		// TODO: Check if this is always correct
		chassisInfo.type = XML::GetFirstChildElementText(element, "description");
		gComponents["CHASSIS"].MergeWith(chassisInfo);
	}

	element = XML::GetElementByAttribute(doc, "id", "core");
	if (element != NULL) {
		Component boardInfo;
		boardInfo.name = XML::GetFirstChildElementText(element, "product");
		boardInfo.vendor = XML::GetFirstChildElementText(element, "vendor");
		boardInfo.serial = XML::GetFirstChildElementText(element, "serial");
		gComponents["BOARD"].MergeWith(boardInfo);
	}

	element = XML::GetElementByAttribute(doc, "id", "display");
	if (element != NULL) {
		// TODO: there could be multiple displays
		video_info info;
		info.name = XML::GetFirstChildElementText(element, "description");
		info.vendor = XML::GetFirstChildElementText(element, "vendor");
		info.chipset = XML::GetFirstChildElementText(element, "product");
		if (fVideoInfo.size() == 0)
			fVideoInfo.push_back(info);
		else
			fVideoInfo[0].MergeWith(info);
	}

	if (fMemoryInfo.size() == 0) {
		const size_t memoryLength = ::strlen("memory");
		element = XML::GetElementByAttribute(doc, "id", "memory", XML::match_partial);
		while (element != NULL) {
			std::string memoryCaption;
			tmpElement = element->FirstChildElement("description");
			if (tmpElement != NULL)
				memoryCaption = tmpElement->GetText();
			const tinyxml2::XMLElement* bankElement
				= XML::GetElementByAttribute(*element, "id", "bank", XML::match_partial);
			if (bankElement == NULL) {
				// In some cases (VMs for example), there is no "bank" element
				memory_device_info info;
				info.caption = memoryCaption;
				info.purpose = info.caption;
				tmpElement = element->FirstChildElement("size");
				if (tmpElement != NULL) {
					memory_device_info info;
					info.caption = memoryCaption;
					info.purpose = info.caption;
					info.size = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
					fMemoryInfo.push_back(info);
				}
			} else {
				while (bankElement != NULL) {
					memory_device_info info;
					info.caption = memoryCaption;
					info.purpose = info.caption;
					info.description = XML::GetFirstChildElementText(bankElement, "description");
					info.type = RAM_type_from_description(info.description);
					info.serial = XML::GetFirstChildElementText(bankElement, "serial");

					tmpElement = bankElement->FirstChildElement("clock");
					if (tmpElement != NULL) {
						// In Hz, usually, but we should check the unit
						info.speed = ::strtoul(tmpElement->GetText(), NULL, 10) / (1000 * 1000);
					}
					tmpElement = bankElement->FirstChildElement("size");
					if (tmpElement != NULL) {
						info.size = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
						fMemoryInfo.push_back(info);
					}
					bankElement = bankElement->NextSiblingElement();
				}
			}

			element = element->NextSiblingElement();
			if (element != NULL) {
				if (::strncasecmp(element->Attribute("id"), "memory", memoryLength) != 0)
					break;
			}
		}
	}

	return true;
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


void
video_info::MergeWith(const video_info& info)
{
	if (name.empty())
		name = info.name;
	if (vendor.empty())
		vendor = info.vendor;
	if (chipset.empty())
		chipset = info.chipset;
	if (memory.empty())
		memory = info.memory;
	if (resolution.empty())
		resolution = info.resolution;
}


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
