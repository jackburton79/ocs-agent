/*
 * CPUINFOBACKEND.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef CPUINFOBACKEND_H_
#define CPUINFOBACKEND_H_

#include "DataBackend.h"

#include <map>
#include <string>


class CPUInfoBackend : public DataBackend {
public:
	CPUInfoBackend();
	virtual bool IsAvailable() const;
	virtual int Run();
};

#endif // CPUINFOBACKEND_H_
