/*
 * OSInfoBackend.h
 *
 *  Created on: 24 gen 2022
 *      Author: Stefano Ceccherini
 */

#ifndef BACKENDS_OSINFOBACKEND_H_
#define BACKENDS_OSINFOBACKEND_H_

#include "DataBackend.h"

class OSInfoBackend : public DataBackend {
public:
	OSInfoBackend();
	virtual ~OSInfoBackend();
    virtual int Run();
};


#endif /* BACKENDS_OSINFOBACKEND_H_ */
