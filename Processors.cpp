/*
 * Processors.cpp
 *
 *      Author: Stefano Ceccherini
 */

#include "Processors.h"

#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <map>

Processors::Processors()
{
	_GetCPUInfo();
	Rewind();
}


void
Processors::_GetCPUInfo()
{
	ProcReader cpuReader("/proc/cpuinfo");
	std::istream iStream(&cpuReader);

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
						::strtol(value.c_str(), NULL, 0);
				else if (name == "cache size")
					tmpCPUInfo[processorNum].cache_size = value;
			} catch (...) {
			}
		}
	}

	std::map<int, processor_info>::const_iterator i;
	for (i = tmpCPUInfo.begin(); i != tmpCPUInfo.end(); i++) {
		const processor_info& cpu = i->second;
		int CPUPhysID = cpu.physical_id;
		processor_info processorInfo;
		processorInfo.type = cpu.type;
		processorInfo.speed = cpu.speed;
		processorInfo.cores = cpu.cores;
		processorInfo.manufacturer = cpu.manufacturer;
		processorInfo.cache_size = cpu.cache_size;
		if (size_t(CPUPhysID) >= fItems.size())
			fItems.push_back(processorInfo);
		else {
			// TODO: Find out why it doesn't recognize the [] operator
			std::list<processor_info>::iterator it = fItems.begin();
			std::advance(it, CPUPhysID);
			*it = processorInfo;
		}
	}
}


std::string
processor_info::Speed() const
{
	std::string mhz = speed;

	size_t pos = mhz.find(".");
	if (pos != std::string::npos) {
		mhz = mhz.substr(0, pos);
	}

	return mhz;
}
