/*
 * ProcReader.cpp
 *
 *  Created on: 15/lug/2013
 *      Author: stefano
 */

#include "ProcReader.h"

#include <iostream>
#include <string>


#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// TODO: for now it works, but make this class more like the popen_streambuf
ProcReader::ProcReader(const char* sub)
{
	std::string fullName = "/proc/";
	fullName.append(sub);

	fFD = ::open(fullName.c_str(), O_RDONLY);
}


ProcReader::~ProcReader()
{
	::close(fFD);
}


std::string
ProcReader::ReadLine()
{
	std::string line;
	char buffer[256];
	size_t readAmount = ::read(fFD, buffer, sizeof(buffer));

	if (readAmount > 0)
		line.append(buffer, readAmount - 1);

	return line;
}
