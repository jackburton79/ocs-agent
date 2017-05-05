/*
 * Softwares.h
 *
 *      Author: Jonathan ORSEL
 */

#ifndef SOFTWARES_H_
#define SOFTWARES_H_

#include <string>
#include <vector>

struct software_info {
	std::string comments;
	std::string size;
	std::string from;
	std::string installdate;
	std::string name;
	std::string version;
};

class Softwares {
public:
	Softwares();
	~Softwares();

	int Count() const;
	software_info SoftwareAt(int i) const;

private:
	void _ReadSoftwaresInfo();

	std::vector<software_info> fSoftwares;

};

#endif /* SOFTWARES_H_*/
