/*
 * Support.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include <stdio.h>
#include <streambuf>

class popen_streambuf : public std::streambuf {
public:
	popen_streambuf();
	virtual ~popen_streambuf();

	popen_streambuf* open(const char* command, const char* mode);
	void close();
	std::streamsize xsgetn(char_type* ptr, std::streamsize n);
	int_type underflow();
	std::streamsize showmanyc();
private:
	FILE* fFile;
	char_type* fBuffer;
};

#endif /* SUPPORT_H_ */
