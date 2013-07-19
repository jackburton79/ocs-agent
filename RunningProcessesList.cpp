/*
 * RunningProcesses.cpp
 *
 *  Created on: 16/lug/2013
 *      Author: stefano
 */

#include "ProcReader.h"
#include "RunningProcessesList.h"
#include "Support.h"

#include <dirent.h>
#include <stdlib.h>

#include <iostream>

static bool
IsNumber(std::string string)
{
	return !string.empty()
			&& string.find_first_not_of("0123456789") == std::string::npos;
}


RunningProcessesList::RunningProcessesList()
{
	DIR* dir = ::opendir("/proc/");
	if (dir != NULL) {
		dirent* entry = NULL;
		while ((entry = ::readdir(dir)) != NULL) {
			std::string procPid = entry->d_name;
			if (IsNumber(procPid)) {
				std::string fullName = "/proc/";
				fullName.append(procPid);
				process_info info;
				_ReadProcessInfo(info, procPid.c_str());
				fItems.push_back(info);
			}
		}
		::closedir(dir);
	}

	Rewind();
}


RunningProcessesList::~RunningProcessesList()
{
}



void
RunningProcessesList::_ReadProcessInfo(process_info& info, std::string pid)
{
	info.pid = strtol(pid.c_str(), NULL, 10);
	info.cmdline = ProcReader((pid + std::string("/cmdline")).c_str()).ReadLine();

	// TODO: Refactor, too much duplicated code
	ProcReader status((pid + std::string("/status")).c_str());

	std::istream stream(&status);
	std::string line;
	try {
		while (std::getline(stream, line) > 0) {
			if (line.find("VmSize") != std::string::npos) {
				size_t pos = line.find(":");
				if (pos == std::string::npos)
					continue;
				std::string valueString = line.substr(pos + 2, std::string::npos);
				info.memory = strtol(trim(valueString).c_str(), NULL, 10);
			} else if (line.find("VmSwap") != std::string::npos) {
				size_t pos = line.find(":");
				if (pos == std::string::npos)
					continue;
				std::string valueString = line.substr(pos + 2, std::string::npos);
				info.virtualmem = strtol(trim(valueString).c_str(), NULL, 10);
			}
		}
	} catch (...) {

	}

}
