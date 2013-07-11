/*
 * Machine.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <string>

class Machine {
public:
	Machine();
	~Machine();

	std::string BIOSManufacturer() const;
	std::string BIOSDate() const;
	std::string BIOSVersion() const;
	std::string MachineModel() const;
	std::string MachineManufacturer() const;
	std::string SerialNumber() const;

private:
	void _GetDMIDecodeData();
	void _GetBIOSInfo(std::istream& stream);
	void _GetSystemInfo(std::istream& stream);

	std::string fBiosDate;
	std::string fBiosManufacturer;
	std::string fBiosVersion;

	std::string fMachineModel;
	std::string fMachineManufacturer;
	std::string fSerialNumber;
};

#endif /* MACHINE_H_ */
