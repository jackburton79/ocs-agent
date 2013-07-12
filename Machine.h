/*
 * Machine.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <map>
#include <string>

class Machine {
public:
	Machine();
	~Machine();

	std::string BIOSManufacturer() const;
	std::string BIOSDate() const;
	std::string BIOSVersion() const;
	std::string SystemModel() const;
	std::string MachineManufacturer() const;
	std::string SystemSerialNumber() const;

private:
	void _GetDMIDecodeData();
	void _GetBIOSInfo(std::istream& stream);
	void _GetSystemInfo(std::istream& stream);

	std::string _GetBIOSValue(std::string string) const;
	std::string _GetSystemValue(std::string string) const;

	std::map<std::string, std::string> fBIOSInfo;
	std::map<std::string, std::string> fSystemInfo;
};

#endif /* MACHINE_H_ */
