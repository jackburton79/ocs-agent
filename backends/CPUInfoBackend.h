/*
 * CPUINFOBACKEND.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef CPUINFOBACKEND_H_
#define CPUINFOBACKEND_H_

#include "DataBackend.h"

#include <string>


class CPUInfoBackend : public DataBackend {
public:
	CPUInfoBackend();
	virtual int Run();
private:
	void _GetCPUInfo();
};

#endif // CPUINFOBACKEND_H_
