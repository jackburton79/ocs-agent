/*
 * Softwares.h
 *
 *      Author: Jonathan ORSEL
 */

#ifndef SOFTWARES_H_
#define SOFTWARES_H_

#include <string>

#include "ItemsList.h"

struct software_info {
	std::string comments;
	std::string size;
	std::string from;
	std::string installdate;
	std::string name;
	std::string version;
};

class Softwares : public ItemsList<software_info> {
public:
	Softwares();

private:
	void _ReadSoftwaresInfo();

};

#endif /* SOFTWARES_H_*/
