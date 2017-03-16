/*
 * Storages.cpp
 *
 *      Author: Stefano Ceccherini
 */

#include "Storages.h"
#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <istream>

Storages::Storages()
{
	_ReadStoragesInfo();	
}

Storages::~Storages()
{
}


int
Storages::Count() const
{
	return fStorages.size();
}


storage_info
Storages::StorageAt(int i) const
{
	return fStorages[i];
}


void
Storages::_ReadStoragesInfo()
{
	try {
		ProcReader procReader("scsi/scsi");

		std::istream stream(&procReader);
        	std::string line;
        
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
			fStorages.push_back(info);
                }
        } catch (...) {
        	std::cout << "No storage info" << std::endl;
        }
}

