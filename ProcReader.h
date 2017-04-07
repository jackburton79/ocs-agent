/*
 * ProcReader.h
 *
 *  Created on: 15/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef PROCREADER_H_
#define PROCREADER_H_

#include <algorithm>
#include <stdio.h>
#include <string>
#include <streambuf>

class ProcReader : public std::streambuf {
public:
	ProcReader(const char* fullPath);
	virtual ~ProcReader();

	ProcReader* open(const char* command, const char* mode);
	void close();
	std::streamsize xsgetn(char_type* ptr, std::streamsize n);
	int_type underflow();
	std::streamsize showmanyc();
	std::string ReadLine();
private:
	int fFD;
	char_type* fBuffer;
};

#endif /* PROCREADER_H_ */
