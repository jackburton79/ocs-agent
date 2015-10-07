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
#include <vector>

struct os_info {
	std::string comments;
	std::string hostname;
	std::string machine;
	std::string domain_name;
	std::string os_release;
	std::string os_description;
	std::string memory;
	std::string swap;
};


struct video_info {
	std::string chipset;
	std::string memory;
	std::string name;
	std::string resolution;
};


class Machine {
public:
	static Machine* Get();

	std::string AssetTag() const;
	std::string BIOSManufacturer() const;
	std::string BIOSDate() const;
	std::string BIOSVersion() const;
	std::string MachineManufacturer() const;
	std::string MachineSerialNumber() const;
	std::string SystemModel() const;
	std::string SystemSerialNumber() const;
	std::string SystemUUID() const;
	std::string SystemManufacturer() const;

	std::string HostName() const;

	int CountProcessors() const;
	std::string ProcessorSpeed(int numCpu) const;
	std::string ProcessorManufacturer(int numCpu) const;
	std::string ProcessorSerialNumber(int numCpu) const;
	std::string ProcessorType(int numCpu) const;

	int CountMemories();
	std::string MemoryID(int num);
	std::string MemoryCaption(int num);
	std::string MemoryDescription(int num);
	std::string MemoryCapacity(int num);
	std::string MemoryPurpose(int num);
	std::string MemoryType(int num);
	std::string MemorySpeed(int num);
	std::string MemoryNumSlots(int num);
	std::string MemorySerialNumber(int num);

	os_info OSInfo() const;

	int CountVideos() const;
	video_info VideoInfoFor(int numVideo) const;

private:
	Machine();
	~Machine();

	void _RetrieveData();
	bool _GetDMIDecodeData();
	bool _GetLSHWShortData();
	bool _GetLSHWData();
	void _GetCPUInfo();
	void _GetOSInfo();
	void _IdentifyOS();
	void _GetSystemInfo(std::istream& stream, std::string header);

	std::string _GetValue(std::string string, std::string header) const;
	std::vector<std::string> _GetValues(std::string string, std::string header) const;

	std::string _ProcessorInfo(const char* info, int num) const;

	// TODO: Use a std::vector, more than 16 cpu aren't uncommon nowadays
	int fNumCPUs;
	std::map<std::string, std::string> fCPUInfo[16];

	std::multimap<std::string, std::string> fSystemInfo;
	os_info fKernelInfo;

	std::vector<struct video_info> fVideoInfo;
};

#endif /* MACHINE_H_ */
