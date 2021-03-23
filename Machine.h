/*
 * Machine.h
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <map>
#include <string>
#include <vector>

#include "backends/DataBackend.h"

struct memory_device_info {
	memory_device_info();

	std::string Type() const;
	std::string Speed() const;
	std::string Size() const;

	std::string description;
	std::string caption;
	std::string purpose;
	std::string type;
	std::string vendor;
	std::string serial;
	std::string asset_tag;
	unsigned int speed;
	unsigned int size;
};


class OSInfoBackend : public DataBackend {
public:
	OSInfoBackend();
	virtual ~OSInfoBackend();
    virtual int Run();
};


class Component {
public:
	std::map<std::string, std::string> fields;
	void MergeWith(Component& component);
};

typedef std::map<std::string, Component> components_map;
extern components_map gComponents;

class Machine {
public:
	static Machine* Get();

	int CountMemories();
	std::string MemoryCaption(int num);
	std::string MemoryDescription(int num);
	std::string MemoryCapacity(int num);
	std::string MemoryPurpose(int num);
	std::string MemoryType(int num);
	std::string MemorySpeed(int num);
	std::string MemoryNumSlot(int num);
	std::string MemorySerialNumber(int num);

private:
	Machine();
	~Machine();

	std::vector<memory_device_info> fMemoryInfo;
};

#endif /* MACHINE_H_ */
