/*
 * CPU.h
 *
 *      Author: Stefano Ceccherini
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "DataBackend.h"

#include <string>


class Processors : public DataBackend {
public:
	Processors();
private:
	void _GetCPUInfo();
};

#endif // PROCESSOR_H_
