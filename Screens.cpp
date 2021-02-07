/*
 * Screens.cpp
 *
 *  Created on: 15/lug/2015
 *      Author: Stefano Ceccherini
 */

#include "Screens.h"

#include "EDID.h"
#include "Support.h"

#include <cstring>
#include <iostream>
#include <string>

struct pnp_id {
	const char* manufacturer;
	const char* id;
	bool operator==(const pnp_id& a) const { return ::strcasecmp(a.id, id) == 0; };
	bool operator()(const pnp_id& a, const pnp_id& b) const { return ::strcasecmp(a.id, b.id) < 0; };
};

static struct pnp_id kPNPIDs[] = {
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
	const size_t numElements = sizeof(kPNPIDs) / sizeof(kPNPIDs[0]);
	std::sort(kPNPIDs, kPNPIDs + numElements, kPNPIDs[0]);
	Rewind();
}


std::string
GetManufacturerFromID(const std::string& string)
{
	const size_t numElements = sizeof(kPNPIDs) / sizeof(kPNPIDs[0]);
	const struct pnp_id key = { "dummy", string.c_str() };
	const pnp_id* element = std::find(kPNPIDs, kPNPIDs + numElements, key);

	return element->manufacturer;
}
