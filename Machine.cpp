/*
 * Machine.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */


#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include <tinyxml2/tinyxml2.h>

#include <XML.h>

typedef std::map<std::string, std::string> string_map;

components_map gComponents;


void
Component::MergeWith(Component& component)
{
	fields.insert(component.fields.begin(), component.fields.end());
};



// OSInfoBackend
OSInfoBackend::OSInfoBackend()
{
}


/* virtual */
OSInfoBackend::~OSInfoBackend()
{
}


/* virtual */
int
OSInfoBackend::Run()
{
	std::string line;
	std::string osDescription;
	Component os;
	if (::access("/etc/os-release", F_OK) != -1) {
		ProcReader osReader("/etc/os-release");
		try {
			while ((line = osReader.ReadLine()) != "") {
				if (line.find("PRETTY_NAME") != std::string::npos) {
					size_t pos = line.find('=');
					if (pos != std::string::npos) {
						std::string value = line.substr(pos + 1, std::string::npos);
						value = trim(value);
						// remove quotes
						osDescription = value.substr(1, value.length() - 2);
						break;
					}
				}
			}
		} catch (...) {
			// not an error
		}
	} else if (CommandExists("lsb_release")) {
		CommandStreamBuffer lsb;
		lsb.open("lsb_release -a", "r");
		std::istream lsbStream(&lsb);
		while (std::getline(lsbStream, line)) {
			if (line.find("Description") != std::string::npos) {
				size_t pos = line.find(':');
				if (pos != std::string::npos) {
					std::string value = line.substr(pos + 1, std::string::npos);
					osDescription = trim(value);
					break;
				}
			}
		}
	} else if (::access("/etc/thinstation.global", F_OK) != -1) {
		osDescription = "Thinstation";
		char* tsVersion = ::getenv("TS_VERSION");
		if (tsVersion != NULL) {
			osDescription += " ";
			osDescription += tsVersion;
		}
	} else {
		try {
			osDescription = trimmed(ProcReader("/etc/redhat-release").ReadLine());
		} catch (...) {
			osDescription = "Unknown";
		}
	}

	os.fields["description"] = osDescription;
	gComponents.Merge("OS", os);
	return 0;
}
