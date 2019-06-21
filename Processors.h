/*
 * CPU.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "ItemsList.h"

#include <string>

struct processor_info {
	int physical_id;
	std::string manufacturer;
	std::string type;
	std::string Speed() const;
	std::string cores;
	std::string cache_size;
	std::string serial;
	std::string speed;
};

class Processors : public ItemsList<processor_info> {
public:
	Processors();
private:
	void _GetCPUInfo();
};

#endif // PROCESSOR_H_
