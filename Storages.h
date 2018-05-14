/*
 * Storages.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef STORAGES_H_
#define STORAGES_H_

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

class Storages : public ItemsList<storage_info> {
public:
	Storages();

private:
	void _ReadStoragesInfo();
};

#endif /* STORAGES_H_*/
