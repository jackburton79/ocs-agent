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


// TODO: This class is very inefficient:
// It iterates the full dmi_db for every call of CountEntries() or ExtractEntry()
class DMIExtractor {
public:
	DMIExtractor(dmi_db db);
	int CountEntries(const std::string& context) const;
	std::vector<string_map> ExtractEntry(const std::string& context) const;
	string_map ExtractHandle(std::string handle) const;
private:
	dmi_db fDMIDB;
};


DMIExtractor::DMIExtractor(dmi_db db)
	:
	fDMIDB(db)
{
}


int
DMIExtractor::CountEntries(const std::string& context) const
{
	dmi_db::const_iterator dbIterator;
	int count = 0;
	for (dbIterator = fDMIDB.begin(); dbIterator != fDMIDB.end(); dbIterator++) {
		string_map entry = (*dbIterator).second;
		string_map::const_iterator i = entry.find("NAME");
		if (i != entry.end() && i->second == context) {
			count++;
		}
	}

	return count;
}


std::vector<string_map>
DMIExtractor::ExtractEntry(const std::string& context) const
{
	dmi_db::const_iterator dbIterator;
	string_map entry;
	std::vector<string_map> entries;
	for (dbIterator = fDMIDB.begin(); dbIterator != fDMIDB.end(); dbIterator++) {
		entry = dbIterator->second;
		string_map::const_iterator i = entry.find("NAME");
		if (i != entry.end() && i->second == context)
			entries.push_back(entry);
	}

	return entries;
}


string_map
DMIExtractor::ExtractHandle(std::string handle) const
{
	int numericHandle = strtol(handle.c_str(), NULL, 0);
	dmi_db::const_iterator i = fDMIDB.find(numericHandle);
	if (i == fDMIDB.end())
		throw std::runtime_error("DMIExtractor::ExtractHandle: cannot find requested handle");
	return i->second;
}


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


// Returns a string containing the size, in MBytes,
// starting from a string like '3GB' or '1024 KB'
std::string
convert_to_MBytes(std::string string)
{
	char *memoryUnit = NULL;
	int memorySize = ::strtol(string.c_str(), &memoryUnit, 10);
	std::string unit = memoryUnit;
	trim(unit);
	if (::strcasecmp(unit.c_str(), "KB") == 0
		|| ::strcasecmp(unit.c_str(), "KiB") == 0)
		memorySize /= 1024;
	else if (::strcasecmp(unit.c_str(), "GB") == 0
		 || ::strcasecmp(unit.c_str(), "GiB") == 0)
		memorySize *= 1024;
	return int_to_string(memorySize);
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
		_GetGraphicsCardInfo();
		_GetDMIDecodeData();
		_GetLSHWData();
		_GetCPUInfo();
		_GetOSInfo();
	} catch (...) {
		std::cerr << "Failed to get hardware info." << std::endl;
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
Machine::Architecture() const
{
	return fKernelInfo.machine;
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
	return fSystemInfo.name;
}


std::string
Machine::SystemSerialNumber() const
{
	return fSystemInfo.serial;
}


std::string
Machine::SystemUUID() const
{
	return fSystemInfo.uuid;
}


std::string
Machine::SystemType() const
{
	return fChassisInfo.type;
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
	return fCPUInfo[numCpu].serial;
}


std::string
Machine::ProcessorType(int numCpu) const
{
	return fCPUInfo[numCpu].type;
}


std::string
Machine::ProcessorCores(int numCpu) const
{
	return fCPUInfo[numCpu].cores;
}


std::string
Machine::ProcessorCacheSize(int numCpu) const
{
	return fCPUInfo[numCpu].cache_size;
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
	try {
		fBIOSInfo.release_date = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_date").ReadLine());
		fBIOSInfo.vendor = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_vendor").ReadLine());
		fBIOSInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/bios_version").ReadLine());
		
		fSystemInfo.name = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_name").ReadLine());
		fSystemInfo.version = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_version").ReadLine());
		fSystemInfo.uuid = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_uuid").ReadLine());
		fSystemInfo.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/product_serial").ReadLine());
		
		fChassisInfo.asset_tag = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_asset_tag").ReadLine());
		fChassisInfo.serial = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_serial").ReadLine());
		//fChassisInfo.type = trimmed(ProcReader("/sys/devices/virtual/dmi/id/chassis_type").ReadLine());
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
Machine::_GetGraphicsCardInfo()
{
	try {
		struct video_info videoInfo;
		videoInfo.name = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_product_name").ReadLine());
		videoInfo.vendor = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_vendor").ReadLine());
		videoInfo.chipset = trimmed(ProcReader("/sys/class/graphics/fb0/device/oem_string").ReadLine());
		videoInfo.resolution = trimmed(ProcReader("/sys/class/graphics/fb0/virtual_size").ReadLine());
		std::replace(videoInfo.resolution.begin(), videoInfo.resolution.end(), ',', 'x');

		fVideoInfo.push_back(videoInfo);
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
		dmi_db dmiDatabase;

		int numericHandle = 0;
		bool insideHandle = false;
		while (std::getline(iStream, string)) {
			if (string.find("Handle") == 0) {
				size_t numStart = string.find_first_of(" 0");
				size_t numEnd = string.find_first_of(",", numStart);
				std::string handle = string.substr(numStart + 1, numEnd - numStart - 1);

				numericHandle = strtol(handle.c_str(), NULL, 16);

				std::string name;
				std::getline(iStream, name);

				std::map<std::string, std::string> dbEntry;
				dbEntry["DMIHANDLE"] = trimmed(handle);
				dbEntry["NAME"] = trimmed(name);

				dmiDatabase[numericHandle] = dbEntry;
				insideHandle = true;

			} else if (insideHandle) {
				if (string.empty() || string == "") {
					insideHandle = false;
					continue;
				}

				size_t pos = string.find(":");
				if (pos == std::string::npos)
					continue;
				try {
					std::string name = trimmed(string.substr(0, pos));
					std::string value = trimmed(string.substr(pos + 2, std::string::npos));
					dmiDatabase[numericHandle][name] = value;
				} catch (...) {

				}
			}
		}

		_ExtractDataFromDMIDB(dmiDatabase);
	} catch (...) {
		return false;
	}

	return true;
}


bool
Machine::_GetLSHWData()
{
	if (!CommandExists("lshw"))
		return false;

	// Load command output into "string"
	popen_streambuf lshw("lshw -xml", "r");
	std::istream iStream(&lshw);
	std::istreambuf_iterator<char> eos;
	std::string string(std::istreambuf_iterator<char>(iStream), eos);

	tinyxml2::XMLDocument doc;
	if (doc.Parse(string.c_str(), string.size()) != tinyxml2::XML_SUCCESS)
		return false;

	const tinyxml2::XMLElement* tmpElement = NULL;
	const tinyxml2::XMLElement* element = XML::GetElementByAttribute(doc, "id", "firmware");
	if (element != NULL) {
		if (fBIOSInfo.release_date.empty()) {
			tmpElement = element->FirstChildElement("date");
			if (tmpElement != NULL)
				fBIOSInfo.release_date = tmpElement->GetText();
		}
		if (fBIOSInfo.vendor.empty()) {
			tmpElement = element->FirstChildElement("vendor");
			if (tmpElement != NULL)
				fBIOSInfo.vendor = tmpElement->GetText();
		}
		if (fBIOSInfo.version.empty()) {
			tmpElement = element->FirstChildElement("version");
			if (tmpElement != NULL)
				fBIOSInfo.version = tmpElement->GetText();
		}
	}

	element = XML::GetElementByAttribute(doc, "class", "system");
	if (element != NULL) {
		if (fSystemInfo.name.empty()) {
			tmpElement = element->FirstChildElement("product");
			if (tmpElement != NULL)
				fSystemInfo.name = tmpElement->GetText();
		}
		if (fSystemInfo.version.empty()) {
			tmpElement = element->FirstChildElement("version");
			if (tmpElement != NULL)
				fSystemInfo.version = tmpElement->GetText();
		}
		if (fSystemInfo.serial.empty()) {
			tmpElement = element->FirstChildElement("serial");
			if (tmpElement != NULL)
				fSystemInfo.serial = tmpElement->GetText();
		}
		if (fSystemInfo.vendor.empty()) {
			tmpElement = element->FirstChildElement("vendor");
			if (tmpElement != NULL)
				fSystemInfo.vendor = tmpElement->GetText();
		}
		// TODO: Check if this is always correct
		if (fChassisInfo.type.empty()) {
			tmpElement = element->FirstChildElement("description");
			if (tmpElement != NULL)
				fChassisInfo.type = tmpElement->GetText();
		}
	}

	element = XML::GetElementByAttribute(doc, "id", "core");
	if (element != NULL) {
		if (fBoardInfo.name.empty()) {
			tmpElement = element->FirstChildElement("product");
			if (tmpElement != NULL)
				fBoardInfo.name = tmpElement->GetText();
		}
		if (fBoardInfo.vendor.empty()) {
			tmpElement = element->FirstChildElement("vendor");
			if (tmpElement != NULL)
				fBoardInfo.vendor = tmpElement->GetText();
		}
		if (fBoardInfo.serial.empty()) {
			tmpElement = element->FirstChildElement("serial");
			if (tmpElement != NULL)
				fBoardInfo.serial = tmpElement->GetText();
		}
	}

	if (fVideoInfo.size() == 0) {
		element = XML::GetElementByAttribute(doc, "id", "display");
		if (element != NULL) {
			// TODO: there could be multiple displays
			video_info info;
			tmpElement = element->FirstChildElement("description"); 
			if (tmpElement != NULL)
				info.name = tmpElement->GetText();
			tmpElement = element->FirstChildElement("vendor");
			if (tmpElement != NULL)
				info.vendor = tmpElement->GetText();
			tmpElement = element->FirstChildElement("product");
			if (tmpElement != NULL)
				info.chipset = tmpElement->GetText();
			fVideoInfo.push_back(info);
		}
	}

	if (fMemoryInfo.size() == 0) {
		element = XML::GetElementByAttribute(doc, "id", "memory", false);
		while (element != NULL) {
			std::string memoryCaption;
			tmpElement = element->FirstChildElement("description");
			if (tmpElement != NULL)
				memoryCaption = tmpElement->GetText();
			const tinyxml2::XMLElement* bankElement
				= XML::GetElementByAttribute(*element, "id", "bank", false);
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
					unsigned int numBytes = strtol(tmpElement->GetText(), NULL, 10);
					info.size = int_to_string(numBytes / (1024 * 1024));
					fMemoryInfo.push_back(info);
				}
			} else {
				while (bankElement != NULL) {
					memory_device_info info;
					info.caption = memoryCaption;
					info.purpose = info.caption;
					tmpElement = bankElement->FirstChildElement("description");
					if (tmpElement != NULL) {
						info.description = tmpElement->GetText();
						// TODO: Not the cleanest approach, but lshw doesn't
						// seem to return this in any other field
						if (info.description.find("SDRAM") != std::string::npos)
							info.type = "SDRAM";
						else if (info.description.find("FLASH") != std::string::npos)
							info.type = "FLASH";
						else if (info.description.find("DDR") != std::string::npos)
							info.type = "DDR";
						// TODO: Yeah, and DDR2 ? DDR3 ?
					}
					tmpElement = bankElement->FirstChildElement("serial");
					if (tmpElement != NULL) {
						info.serial = tmpElement->GetText();
					}

					tmpElement = bankElement->FirstChildElement("clock");
					if (tmpElement != NULL) {
						// In Hz, usually, but we should check the unit
						unsigned int speed = strtol(tmpElement->GetText(), NULL, 10);
						info.speed = int_to_string(speed / (1000 * 1000));
					}
					tmpElement = bankElement->FirstChildElement("size");
					if (tmpElement != NULL) {
						unsigned int numBytes = strtol(tmpElement->GetText(), NULL, 10);
						info.size = int_to_string(numBytes / (1024 * 1024));
						fMemoryInfo.push_back(info);
					}
					bankElement = bankElement->NextSiblingElement();
				}
			}

			element = element->NextSiblingElement();
			if (element != NULL) {
				if (::strncasecmp(element->Attribute("id"), "memory", strlen("memory")) != 0)
					break;
			}
		}
		
	}
	
	return true;
}


void
Machine::_GetCPUInfo()
{
	ProcReader cpu("/proc/cpuinfo");
	std::istream iStream(&cpu);

	// First pass: we get every processor info into a map,
	// based on processor number. Then we examine the map and 
	// check if some cpu share the physical_id. If so, we merge
	// them into the same processor (later).
	std::map<int, processor_info> tmpCPUInfo;
	std::string string;
	int processorNum = 0;
	while (std::getline(iStream, string)) {
		if (string.find("processor") != std::string::npos) {
			// Get the processor number
			size_t pos = string.find(":");
			if (pos == std::string::npos)
				continue;
			std::string valueString = string.substr(pos + 2, std::string::npos);
			trim(valueString);
			processorNum = ::strtol(valueString.c_str(), NULL, 10);

			processor_info newInfo;
			newInfo.physical_id = 0;
			newInfo.cores = "1";
			tmpCPUInfo[processorNum] = newInfo;
		} else {
			size_t pos = string.find(":");
			if (pos == std::string::npos)
				continue;

			try {
				std::string name = string.substr(0, pos);
				std::string value = string.substr(pos + 1, std::string::npos);
				trim(name);
				trim(value);
				if (name == "model name")
					tmpCPUInfo[processorNum].type = value;
				else if (name == "cpu MHz")
					tmpCPUInfo[processorNum].speed = value;
				else if (name == "vendor_id")
					tmpCPUInfo[processorNum].manufacturer = value;
				else if (name == "cpu cores")
					tmpCPUInfo[processorNum].cores = value;
				else if (name == "physical id")
					tmpCPUInfo[processorNum].physical_id =
						strtol(value.c_str(), NULL, 0);
				else if (name == "cache size")
					tmpCPUInfo[processorNum].cache_size = value;

			} catch (...) {
			}
		}
	}
	
	std::map<int, processor_info>::const_iterator i;	
	for (i = tmpCPUInfo.begin(); i != tmpCPUInfo.end(); i++) {
		const processor_info& cpu = i->second;
		if (size_t(cpu.physical_id) >= fCPUInfo.size())
			fCPUInfo.resize(cpu.physical_id + 1);
		fCPUInfo[cpu.physical_id].type = cpu.type;
		fCPUInfo[cpu.physical_id].speed = cpu.speed;
		fCPUInfo[cpu.physical_id].cores = cpu.cores;
		fCPUInfo[cpu.physical_id].manufacturer = cpu.manufacturer;
		fCPUInfo[cpu.physical_id].cache_size = cpu.cache_size;
	}
}


void
Machine::_GetOSInfo()
{
	struct utsname uName;
	if (::uname(&uName) != 0) {
		std::string errorString = "Machine::_GetOsInfo(): uname() failed with error ";
		errorString.append(::strerror(errno));
		throw std::runtime_error(errorString);
	}

	fKernelInfo.hostname = uName.nodename;
	fKernelInfo.comments = uName.version;
	fKernelInfo.os_release = uName.release;
	fKernelInfo.domain_name = uName.domainname;
	fKernelInfo.machine = uName.machine;

	//Feed domain name from host name when possible.
	if (fKernelInfo.domain_name == "" || fKernelInfo.domain_name == "(none)") {
		size_t dotPos = fKernelInfo.hostname.find('.');
		if (dotPos != std::string::npos) {
			fKernelInfo.domain_name = fKernelInfo.hostname.substr(dotPos + 1);
		}
	}

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


void
Machine::_ExtractDataFromDMIDB(dmi_db systemInfo)
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
	if (string != "" && fSystemInfo.name == "")
		fSystemInfo.name = string;
	string = GetValueFromMap(systemInfo, "Version", kSystemInfo);
	if (string != "" && fSystemInfo.version == "")
		fSystemInfo.version = string;
	string = GetValueFromMap(systemInfo, "UUID", kSystemInfo);
	if (string != "" && fSystemInfo.uuid == "")
		fSystemInfo.uuid = string;
	string = GetValueFromMap(systemInfo, "Serial Number", kSystemInfo);
	if (string != "" && fSystemInfo.serial == "")
		fSystemInfo.serial = string;
		
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

	std::vector<string_map> valuesVector;
	DMIExtractor dmiExtractor(systemInfo);
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
				info.size = int_to_string(0);

			mapIter = entry.find("Locator");
			if (mapIter != entry.end())
				info.description = mapIter->second;

			mapIter = entry.find("Type");
			if (mapIter != entry.end() && info.size != "0")
				info.type = mapIter->second;
			else
				info.type = "Empty slot";

			mapIter = entry.find("Speed");
			if (mapIter != entry.end()) {
				unsigned int speedInteger = strtoul(mapIter->second.c_str(), NULL, 10);
				info.speed = int_to_string(speedInteger);
			}
			mapIter = entry.find("Manufacturer");
			if (mapIter != entry.end())
				info.vendor = mapIter->second;
			mapIter = entry.find("Asset Tag");
			if (mapIter != entry.end())
				info.asset_tag = mapIter->second;
			mapIter = entry.find("Serial Number");
			if (mapIter != entry.end())
				info.serial= mapIter->second;
			
			mapIter = entry.find("Array Handle");
			if (mapIter != entry.end()) {
				std::string parentHandle = mapIter->second;
				string_map arrayHandle = dmiExtractor.ExtractHandle(parentHandle);
				mapIter = arrayHandle.find("Use");
				if (mapIter != entry.end())
					info.purpose = mapIter->second;
				mapIter = arrayHandle.find("Use");
				if (mapIter != entry.end())
					info.caption = mapIter->second;
			}
		} catch (...) {

		}
		if (info.speed == "")
			info.speed = "0";
		// Make sure we have at least some valid info
		if (info.caption != "" || info.purpose != ""
			|| info.type != "" || info.serial != "" || info.speed != "")
			fMemoryInfo.push_back(info);
	}
}
