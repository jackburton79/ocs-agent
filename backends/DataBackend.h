/*
 * DataBackend.h
 *
 *  Created on: 22 mar 2021
 *      Author: Stefano Ceccherini
 */

#ifndef DATABACKEND_H_
#define DATABACKEND_H_

class DataBackend {
public:
	DataBackend();
	virtual ~DataBackend();

	virtual bool IsAvailable() const { return true; };
	virtual int Run() = 0;
};

#endif /* DATABACKEND_H_ */
