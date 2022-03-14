/*
 * Storages.cpp
 *
 *      Author: Stefano Ceccherini
 */

#include "StorageRoster.h"

#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <istream>


StorageRoster::StorageRoster()
{
	_ReadStoragesInfo();	
}


void
StorageRoster::_ReadStoragesInfo()
{
	try {
		ProcReader procReader("/proc/scsi/scsi");

		std::istream stream(&procReader);

		for (;;) {
			std::string dummy;
			if (!std::getline(stream, dummy))
				break;
			std::string hostLine;
			std::string vendorLine;
			std::string typeLine;
			if (!std::getline(stream, hostLine))
				break;
			if (!std::getline(stream, vendorLine))
				break;
			if (!std::getline(stream, typeLine))
				break;	

			storage_info info;
			info.manufacturer = vendorLine.substr(10, 9);
			trim(info.manufacturer);
			info.model = vendorLine.substr(26, 17); 
			trim(info.model);
			// TODO: Name = Model ?
			info.name = info.model;
			info.type = typeLine.substr(10, 26);
			trim(info.type); 
			fItems.push_back(info);
		}
	} catch (...) {
	}

	Rewind();
}

