/*
 * DMIDecode.cpp
 *
 *  Created on: 6 mar 2021
 *      Author: stefano
 */

#include "DMIDecode.h"
#include "Support.h"

typedef std::map<std::string, std::string> string_map;

DMIDecode::DMIDecode()
{
	// TODO Auto-generated constructor stub

}

DMIDecode::~DMIDecode()
{
	// TODO Auto-generated destructor stub
}


DMIExtractor::DMIExtractor(const dmi_db& db)
	:
	fDMIDB(db)
{
}


int
DMIExtractor::CountEntries(const std::string& context) const
{
	dmi_db::const_iterator dbIterator;
	int count = 0;
	for (dbIterator = fDMIDB.begin(); dbIterator != fDMIDB.end(); dbIterator++) {
		string_map entry = (*dbIterator).second;
		string_map::const_iterator i = entry.find("NAME");
		if (i != entry.end() && i->second == context) {
			count++;
		}
	}

	return count;
}


std::vector<string_map>
DMIExtractor::ExtractEntry(const std::string& context) const
{
	dmi_db::const_iterator dbIterator;
	string_map entry;
	std::vector<string_map> entries;
	for (dbIterator = fDMIDB.begin(); dbIterator != fDMIDB.end(); dbIterator++) {
		entry = dbIterator->second;
		string_map::const_iterator i = entry.find("NAME");
		if (i != entry.end() && i->second == context)
			entries.push_back(entry);
	}

	return entries;
}


string_map
DMIExtractor::ExtractHandle(std::string handle) const
{
	int numericHandle = strtol(handle.c_str(), NULL, 0);
	dmi_db::const_iterator i = fDMIDB.find(numericHandle);
	if (i == fDMIDB.end())
		throw std::runtime_error("DMIExtractor::ExtractHandle: cannot find requested handle");
	return i->second;
}


bool
DMIDecode::_GetDMIDecodeData()
{
	if (!CommandExists("dmidecode"))
		return false;

	try {
		CommandStreamBuffer dmi("dmidecode", "r");
		std::istream iStream(&dmi);
		std::string string;
		dmi_db dmiDatabase;

		int numericHandle = 0;
		bool insideHandle = false;
		while (std::getline(iStream, string)) {
			if (string.find("Handle") == 0) {
				size_t numStart = string.find_first_of(" 0");
				size_t numEnd = string.find_first_of(",", numStart);
				std::string handle = string.substr(numStart + 1, numEnd - numStart - 1);

				numericHandle = ::strtol(handle.c_str(), NULL, 16);

				std::string name;
				std::getline(iStream, name);

				std::map<std::string, std::string> dbEntry;
				dbEntry["DMIHANDLE"] = trimmed(handle);
				dbEntry["NAME"] = trimmed(name);

				dmiDatabase[numericHandle] = dbEntry;
				insideHandle = true;

			} else if (insideHandle) {
				if (string.empty() || string == "") {
					insideHandle = false;
					continue;
				}

				size_t pos = string.find(":");
				if (pos == std::string::npos)
					continue;
				try {
					std::string name = trimmed(string.substr(0, pos));
					std::string value = trimmed(string.substr(pos + 2, std::string::npos));
					dmiDatabase[numericHandle][name] = value;
				} catch (...) {
				}
			}
		}

		_ExtractDataFromDMIDB(dmiDatabase);
	} catch (...) {
		return false;
	}

	return true;
}
