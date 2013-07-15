/*
 * ProcReader.h
 *
 *  Created on: 15/lug/2013
 *      Author: stefano
 */

#ifndef PROCREADER_H_
#define PROCREADER_H_

#include <string>

class ProcReader {
public:
	ProcReader(const char* sub);
	~ProcReader();

	std::string ReadLine();
private:
	int fFD;
};

#endif /* PROCREADER_H_ */
