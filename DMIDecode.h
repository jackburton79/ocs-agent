/*
 * DMIDecode.h
 *
 *  Created on: 6 mar 2021
 *      Author: Stefano Ceccherini
 */

#ifndef DMIDECODE_H_
#define DMIDECODE_H_

#include <map>
#include <string>

typedef std::map<int, std::map<std::string, std::string> > dmi_db;

class DMIDecode {
public:
	DMIDecode();
	~DMIDecode();
private:
	bool _GetDMIDecodeData();
};


// TODO: This class is very inefficient:
// It iterates the full dmi_db for every call of CountEntries() or ExtractEntry()
class DMIExtractor {
public:
	DMIExtractor(const dmi_db& db);
	int CountEntries(const std::string& context) const;
	std::vector<string_map> ExtractEntry(const std::string& context) const;
	string_map ExtractHandle(std::string handle) const;
private:
	dmi_db fDMIDB;
};


#endif /* DMIDECODE_H_ */
