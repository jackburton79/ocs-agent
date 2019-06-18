/*
 * Screens.cpp
 *
 *  Created on: 15/lug/2015
 *      Author: Stefano Ceccherini
 */
 
#include "Screens.h"

#include "EDID.h"
#include "Support.h"

#include <iostream>
#include <string>

struct pnp_id {
	const char* manufacturer;
	const char* id;
	const char* date_added;
};

static const struct pnp_id kPNPIDs[] = {
#include "pnp_ids.h"
};

Screens::Screens()
{
	if (CommandExists("find")) {        
		CommandStreamBuffer buf("find /sys/devices/ -name edid", "r");
		std::istream stream(&buf);
		std::string line;
		while (std::getline(stream, line)) {
			screen_info info;
			edid_info edidInfo;
			info.name = line;
			if (get_edid_info(line.c_str(), &edidInfo) == 0) {
				info.description = edidInfo.description;
				info.manufacturer = GetManufacturerFromID(edidInfo.manufacturer);
				info.type = edidInfo.type;
				info.model = edidInfo.model;
				info.serial_number = edidInfo.serial_number;
				fItems.push_back(info);
			}
		}
	}
	Rewind();
}


std::string
GetManufacturerFromID(const std::string& string)
{
	// TODO: Improve
	for (size_t i = 0; i < sizeof(kPNPIDs) / sizeof(kPNPIDs[0]); i++) {
		if (string.compare(kPNPIDs[i].id) == 0)
			return kPNPIDs[i].manufacturer;
	}
	return string;
}
