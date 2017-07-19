/*
 * Screens.cpp
 *
 *  Created on: 15/lug/2015
 *      Author: Stefano Ceccherini
 */
 
#include "Screens.h"
#include "edid-decode.h"
#include "Support.h"

#include <iostream>
#include <string>

Screens::Screens()
{
	if (CommandExists("find")) {        
		popen_streambuf buf("find /sys/devices/ -name edid", "r");
		std::istream stream(&buf);
		std::string line;
		while (std::getline(stream, line)) {
			screen_info info;
			edid_info edidInfo;
			info.name = line;
			if (get_edid_info(line.c_str(), &edidInfo) == 0) {
				info.description = edidInfo.description;
				info.manufacturer = edidInfo.manufacturer;
				info.model = edidInfo.model;
				info.serial_number = edidInfo.serial_number;
				fItems.push_back(info);
			}
		}
	}
	Rewind();
}


