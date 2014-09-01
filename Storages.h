/*
 * Storages.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef STORAGES_H_
#define STORAGES_H_

#include <string>
#include <vector>

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

class Storages {
public:
	Storages();
	~Storages();

	int Count() const;
	storage_info StorageAt(int i) const;

private:
	void _ReadStoragesInfo();

	std::vector<storage_info> fStorages;

};

#endif /* STORAGES_H_*/
