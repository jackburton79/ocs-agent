/*
 * MemInfoBackend.cpp
 *
 *  Created on: 22 mar 2021
 *      Author: Stefano Ceccherini
 */

#include "MemInfoBackend.h"

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <cstdio>
#include <fstream>
#include <iostream>


MemInfoBackend::MemInfoBackend()
{
}


MemInfoBackend::~MemInfoBackend()
{
}


/* virtual */
int
MemInfoBackend::Run()
{
	ProcReader proc("/proc/meminfo");
	std::istream stream(&proc);

	Component os;
	std::string string;
	while (std::getline(stream, string)) {
		size_t pos;
		if ((pos = string.find("SwapTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string swapString = string.substr(pos + 1, std::string::npos);
			int swapInt = ::strtol(trim(swapString).c_str(), NULL, 10) / 1000;
			os.fields["swap"] = int_to_string(swapInt);
		} else if ((pos = string.find("MemTotal:")) != std::string::npos) {
			pos = string.find(":");
			std::string memString = string.substr(pos + 1, std::string::npos);
			int memInt = ::strtol(trim(memString).c_str(), NULL, 10) / 1000;
			os.fields["memory"] = int_to_string(memInt);
		}
	}

	gComponents.Merge("OS", os);

	return 0;
}
