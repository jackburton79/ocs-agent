/*
 * VolumeReader.h
 *
 *  Created on: 19/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef VOLUMEROSTER_H_
#define VOLUMEROSTER_H_

#include "ItemsList.h"

#include <string>


struct volume_info {
	std::string name;
	std::string create_date;
	std::string filesystem;
	int free;
	std::string label;
	std::string serial;
	int total;
	std::string type;
};


class VolumeRoster : public ItemsList<volume_info> {
public:
	VolumeRoster(const char* options = "");
	~VolumeRoster();
};

#endif /* VOLUMEROSTER_H_ */
