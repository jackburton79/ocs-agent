/*
 * UNameBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#ifndef BACKENDS_UNAMEBACKEND_H_
#define BACKENDS_UNAMEBACKEND_H_

#include "DataBackend.h"

class UnameBackend : public DataBackend {
public:
	UnameBackend();
	virtual ~UnameBackend();
	virtual int Run();
};

#endif /* BACKENDS_UNAMEBACKEND_H_ */
