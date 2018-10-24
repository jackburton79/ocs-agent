/*
 * VolumeReader.cpp
 *
 *  Created on: 19/lug/2013
 *      Author: Stefano Ceccherini
 */

#include <VolumeRoster.h>
#include "Support.h"
#include <iostream>
#include <sstream>

VolumeRoster::VolumeRoster(const char* options)
{
	// TODO:
	std::string string("export LC_ALL=C; ");
	string.append("df ").append(options);
	CommandStreamBuffer df(string.c_str(), "r");
	std::istream stream(&df);

	std::string line;
	std::getline(stream, line); // Skip the first line
	while (std::getline(stream, line)) {
		std::istringstream iss(line);

		volume_info info;
		std::string dummy;
		iss >> info.name;
		iss >> info.total;
		iss >> dummy;
		iss >> info.free;
		iss >> dummy;
		iss >> info.type;

		// TODO: filesystem, other
		fItems.push_back(info);
	}

	Rewind();
}


VolumeRoster::~VolumeRoster()
{
}
