/*
 * Storages.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef STORAGEROSTER_H_
#define STORAGEROSTER_H_

#include <string>

#include "ItemsList.h"

struct storage_info {
	std::string name;
	std::string manufacturer;
	std::string model;
	std::string description;
	std::string type;
	std::string size;
	std::string serial_number;
	std::string firmware;
};

class StorageRoster : public ItemsList<storage_info> {
public:
	StorageRoster();

private:
	void _ReadStoragesInfo();
};

#endif /* STORAGEROSTER_H_*/
