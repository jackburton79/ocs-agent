/*
 * MemInfoBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: Stefano Ceccherini
 */

#ifndef MEMINFOBACKEND_H_
#define MEMINFOBACKEND_H_

#include "DataBackend.h"

class MemInfoBackend : public DataBackend {
public:
	MemInfoBackend();
	virtual ~MemInfoBackend();
	virtual int Run();
};

#endif /* MEMINFOBACKEND_H_ */
