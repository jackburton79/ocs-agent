/*
 * LSHWBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#ifndef LSHWBACKEND_H_
#define LSHWBACKEND_H_

#include "DataBackend.h"

namespace tinyxml2 {
	class XMLDocument;
};

class LSHWBackend : public DataBackend {
public:
	LSHWBackend();
	virtual ~LSHWBackend();

	virtual bool IsAvailable() const;
	virtual int Run();

private:
	void _GetCPUInfo(const tinyxml2::XMLDocument& doc);
};

#endif /* LSHWBACKEND_H_ */
