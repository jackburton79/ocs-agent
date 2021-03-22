/*
 * DMIDataBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#ifndef DMIDATABACKEND_H_
#define DMIDATABACKEND_H_

#include "DataBackend.h"

class DMIDataBackend : public DataBackend {
public:
	DMIDataBackend();
	~DMIDataBackend();

	int Run();
};

#endif /* DMIDATABACKEND_H_ */
