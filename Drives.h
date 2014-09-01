/*
 * Drives.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef DRIVES_H_
#define DRIVES_H_

#include <string>
#include <vector>

struct drive_info {
	std::string name;
	std::string manufacturer;
	std::string model;
	std::string description;
	std::string type;
	std::string size;
	std::string serial_number;
	std::string firmware;
};

class Drives {
public:
	Drives();
	~Drives();

	int Count() const;
	drive_info DriveAt(int i) const;

private:
	void _ReadDrivesInfo();

	std::vector<drive_info> fDrives;

};

#endif /* DRIVES_H_*/
