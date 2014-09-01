/*
 * Drives.cpp
 *
 *      Author: Stefano Ceccherini
 */

#include "Drives.h"
#include "ProcReader.h"
#include "Support.h"

#include <iostream>
#include <istream>

Drives::Drives()
{
	_ReadDrivesInfo();	
}

Drives::~Drives()
{
}


int
Drives::Count() const
{
	return fDrives.size();
}


drive_info
Drives::DriveAt(int i) const
{
	return fDrives[i];
}


void
Drives::_ReadDrivesInfo()
{
	ProcReader procReader("scsi/scsi");

	std::istream stream(&procReader);
        std::string line;
        try {
		for (;;) {
			std::string dummy;
			if (std::getline(stream, dummy) <= 0)
				break;
			std::string hostLine;
			std::string vendorLine;
			std::string typeLine;
			if (std::getline(stream, hostLine) <= 0)
				break;
			if (std::getline(stream, vendorLine) <= 0)
				break;
			if (std::getline(stream, typeLine) <= 0)
				break;	

			drive_info info;
                        info.manufacturer = vendorLine.substr(10, 9);
			trim(info.manufacturer);
			info.model = vendorLine.substr(26, 17); 
                      	trim(info.model);
 
			fDrives.push_back(info);
                }
        } catch (...) {
		std::cout << "Caught exception" << std::endl;
        }
}

