/*
 * UNameBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#ifndef UNAMEBACKEND_H_
#define UNAMEBACKEND_H_

#include "DataBackend.h"

class UnameBackend : public DataBackend {
public:
	UnameBackend();
	virtual ~UnameBackend();
	virtual int Run();
};

#endif /* UNAMEBACKEND_H_ */
