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
#include <string.h>
#include <unistd.h>

ProcReader::ProcReader(const char* sub)
{
	open(sub, "r");
}


ProcReader::~ProcReader()
{
	close();
}


std::string
ProcReader::ReadLine()
{
	std::string line;
	std::istream buf(this);
	std::getline(buf, line);

	return line;
}


ProcReader*
ProcReader::open(const char* fileName, const char* mode)
{
	std::string fullName = "/proc/";
	fullName.append(fileName);

	fFD = ::open(fullName.c_str(), O_RDONLY);
	if (fFD < 0)
		return NULL;

	fBuffer = new char[512];
	if (fBuffer == NULL)
		return NULL;

	setg(fBuffer, fBuffer, fBuffer);
	return this;
}


void
ProcReader::close()
{
	if (fFD >= 0) {
		::close(fFD);
		fFD = -1;
	}

	delete[] fBuffer;
}


std::streamsize
ProcReader::xsgetn(char_type* ptr, std::streamsize num)
{
	std::streamsize howMany = showmanyc();
	if (num < howMany) {
		memcpy(ptr, gptr(), num * sizeof(char_type));
		gbump(num);
		return num;
	}

	memcpy(ptr, gptr(), howMany * sizeof(char_type));
	gbump(howMany);

	if (traits_type::eof() == underflow())
		return howMany;

	return howMany + xsgetn(ptr + howMany, num - howMany);
}


ProcReader::traits_type::int_type
ProcReader::underflow()
{
	if (gptr() == 0)
		return traits_type::eof();

	if (gptr() < egptr())
		return traits_type::to_int_type(*gptr());

	size_t len = ::read(fFD, eback(), sizeof(char_type) * 512);
	setg(eback(), eback(), eback() + sizeof(char_type) * len);
	if (len == 0)
		return traits_type::eof();
	return traits_type::to_int_type(*gptr());
}


std::streamsize
ProcReader::showmanyc()
{
	if (gptr() == 0)
		return 0;
	if (gptr() < egptr())
		return egptr() - gptr();
	return 0;
}
