/*
 * VolumeReader.cpp
 *
 *  Created on: 19/lug/2013
 *      Author: stefano
 */

#include "Support.h"
#include "VolumeReader.h"

#include <iostream>
#include <sstream>

VolumeReader::VolumeReader()
{
	popen_streambuf df("export LC_ALL=C; df", "r");
	std::istream stream(&df);

	std::string line;
	std::getline(stream, line); // Skip the first line
	while (std::getline(stream, line) > 0) {
		std::istringstream iss(line);

		/*try {
			while (iss) {
				std::string s;
				iss >> s;
				std::cout << s << std::endl;
			}
		} catch (...) {

		}*/


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

VolumeReader::~VolumeReader()
{
}

