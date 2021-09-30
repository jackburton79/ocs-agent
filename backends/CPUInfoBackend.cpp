/*
 * Processors.cpp
 *
 *      Author: Stefano Ceccherini
 */

#include "CPUInfoBackend.h"

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <list>
#include <map>


struct processor_info {
       int physical_id;
       std::string manufacturer;
       std::string Manufacturer() const;
       std::string type;
       std::string arch;
       std::string Speed() const;
       std::string cores;
       std::string logical_cpus;
       std::string cache_size;
       std::string serial;
       std::string speed;
};

CPUInfoBackend::CPUInfoBackend()
{
}


/* virtual */
int
CPUInfoBackend::Run()
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
				else if (name == "siblings")
					tmpCPUInfo[processorNum].logical_cpus = value;
			} catch (...) {
			}
		}
	}

	std::list<processor_info> CPUs;
	std::map<int, processor_info>::const_iterator i;
	for (i = tmpCPUInfo.begin(); i != tmpCPUInfo.end(); i++) {
		const processor_info& cpu = i->second;
		// TODO: Review this, it doesn't look correct
		int CPUPhysID = cpu.physical_id;
		processor_info processorInfo;
		processorInfo.type = cpu.type;
		processorInfo.speed = cpu.speed;
		processorInfo.cores = cpu.cores;
		processorInfo.manufacturer = cpu.manufacturer;
		processorInfo.cache_size = cpu.cache_size;
		processorInfo.logical_cpus = cpu.logical_cpus;
		if (size_t(CPUPhysID) >= CPUs.size()) {
			processorInfo.physical_id = CPUPhysID;
			CPUs.push_back(processorInfo);
		} else {
			// TODO: Find out why it doesn't recognize the [] operator
			std::list<processor_info>::iterator it = CPUs.begin();
			std::advance(it, CPUPhysID);
			// TODO: we are overwriting a full processor_info struct with
			// an incomplete one (physical_id is not set here)
			*it = processorInfo;
		}
	}

	// TODO: Multi cpu
	processor_info& cpuInfo = CPUs.back();
	Component cpu;
	cpu.fields["vendor"] = cpuInfo.manufacturer;
	cpu.fields["cores"] = cpuInfo.cores;
	cpu.fields["current_speed"] = cpuInfo.Speed();
	cpu.fields["type"] = cpuInfo.type;
	cpu.fields["cache_size"] = cpuInfo.cache_size;
	gComponents.Merge("CPU", cpu);

	return 0;
}


std::string
processor_info::Manufacturer() const
{
	if (manufacturer == "GenuineIntel")
		return "Intel";

	return manufacturer;
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
