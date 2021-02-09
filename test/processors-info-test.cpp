/*
 * processors-info-test.cpp
 *
 *  Created on: 7 feb 2021
 *      Author: Stefano Ceccherini
 */

#include "Processors.h"

#include <iostream>

int main()
{
	processor_info cpuInfo;
	Processors CPUs;
	while (CPUs.GetNext(cpuInfo)) {
		std::cout << "MANUFACTURER:" << cpuInfo.manufacturer << std::endl;
		std::cout << "SERIAL:" << cpuInfo.serial << std::endl;
		std::cout << "SPEED:" << cpuInfo.Speed() << std::endl;
		std::cout << "TYPE:" << cpuInfo.type << std::endl;
		std::cout << "CORES:" << cpuInfo.cores << std::endl;
		std::cout << "LOGICAL CPUs:" << cpuInfo.logical_cpus << std::endl;
		std::cout << "L2CACHESIZE:" << cpuInfo.cache_size << std::endl;
	}
}




