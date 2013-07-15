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


struct kernel_info {
	std::string comments;
	std::string hostname;
	std::string domain_name;
};


class Machine {
public:
	Machine();
	~Machine();

	void RetrieveData();

	std::string AssetTag() const;
	std::string BIOSManufacturer() const;
	std::string BIOSDate() const;
	std::string BIOSVersion() const;
	std::string SystemModel() const;
	std::string MachineManufacturer() const;
	std::string SystemSerialNumber() const;

	int CountProcessors() const;
	std::string ProcessorSpeed(int numCpu) const;
	std::string ProcessorManufacturer(int numCpu) const;
	std::string ProcessorSerialNumber(int numCpu) const;
	std::string ProcessorType(int numCpu) const;

	kernel_info KernelInfo() const;

	int CountProcesses() const;


private:
	void _GetDMIDecodeData();
	void _GetCPUInfo();
	void _GetKernelInfo();


	void _GetSystemInfo(std::istream& stream, std::string header);

	std::string _GetValue(std::string string, std::string header) const;
	std::string _ProcessorInfo(const char* info, int num) const;

	// TODO: Use a std::vector, more than 16 cpu aren't uncommon nowadays
	int fNumCPUs;
	std::map<std::string, std::string> fCPUInfo[16];
	std::map<std::string, std::string> fSystemInfo;
	kernel_info fKernelInfo;
};

#endif /* MACHINE_H_ */
