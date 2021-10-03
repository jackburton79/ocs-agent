/*
 * processors-info-test.cpp
 *
 *  Created on: 7 feb 2021
 *      Author: Stefano Ceccherini
 */

#include "backends/CPUInfoBackend.h"
#include "backends/DMIDecodeBackend.h"
#include "backends/LSHWBackend.h"
#include "Machine.h"
#include <iostream>

int main()
{
	CPUInfoBackend().Run();
	DMIDecodeBackend().Run();
	LSHWBackend().Run();
	std::pair<components_map::iterator, components_map::iterator> CPUs = gComponents.equal_range("CPU");
	size_t cpuCount = 0;
	for (components_map::iterator i = CPUs.first; i != CPUs.second; i++) {
		Component& cpuInfo = i->second;
		cpuCount++;
		std::cout << "CPU " << cpuCount << ":" << std::endl;
		std::cout << "MANUFACTURER:" << cpuInfo.fields["vendor"] << std::endl;
		std::cout << "SERIAL:" << cpuInfo.fields["serial"] << std::endl;
		std::cout << "SPEED:" << cpuInfo.fields["speed"] << std::endl;
		std::cout << "TYPE:" << cpuInfo.fields["type"] << std::endl;
		std::cout << "CORES:" << cpuInfo.fields["cores"] << std::endl;
		std::cout << "DATA WIDTH:" << cpuInfo.fields["width"] << std::endl;
		std::cout << "LOGICAL CPUs:" << cpuInfo.fields["logical_cpus"] << std::endl;
		std::cout << "VOLTAGE:" << cpuInfo.fields["voltage"] << std::endl;
		std::cout << "L2CACHESIZE:" << cpuInfo.fields["cache_size"] << std::endl;
	}
}




