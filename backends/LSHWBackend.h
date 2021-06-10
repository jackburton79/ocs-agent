/*
 * LSHWBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#ifndef LSHWBACKEND_H_
#define LSHWBACKEND_H_

#include "DataBackend.h"

class LSHWBackend : public DataBackend {
public:
	LSHWBackend();
	virtual ~LSHWBackend();

	virtual bool IsAvailable() const;
	virtual int Run();
};

#endif /* LSHWBACKEND_H_ */
