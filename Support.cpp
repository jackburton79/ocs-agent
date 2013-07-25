/*
 * Support.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 *      Code for popen_streambuf found here:
 *      http://stackoverflow.com/questions/1683051/file-and-istream-connect-the-two
 */


#include "Support.h"

#include <streambuf>
#include <string.h>

#include <tinyxml.h>
#include <zlib.h>

bool
CompressXml(const TiXmlDocument& document, char*& destination, size_t& destLength)
{
	//std::cout << "temp name: " << name << std::endl;
	FILE* temp = tmpfile();
	if (temp == NULL) {
		std::cerr << "CompressXml: cannot create temporary file" << std::endl;
		return false;
	}

	if (!document.SaveFile(temp)) {
		std::cerr << "CompressXml: cannot save XML document" << std::endl;
		fclose(temp);
		return false;

	}
	fseek(temp, 0, SEEK_END);
	size_t fileSize = ftell(temp);
	fseek(temp, 0, SEEK_SET);

	char* data = new char[fileSize];
	fread(data, 1, fileSize, temp);

	fclose(temp);

	destLength = compressBound(fileSize);
	destination = new char[destLength];

	if (int compressStatus = compress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)data, (uLong)fileSize) != Z_OK) {
		std::cerr << "Compress returned error: " << zError(compressStatus) << std::endl;
		delete[] destination;
		return false;
	}

	return true;
}


bool
UncompressXml(const char* source, size_t sourceLen, TiXmlDocument& document)
{
	FILE* temp = tmpfile();
	if (temp == NULL) {
		std::cerr << "UncompressXml: cannot create temporary file" << std::endl;
		return false;
	}

	char* destination = new char[32768];
	size_t destLength = 32768;

	if (int status = uncompress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)source, (uLong)sourceLen) != Z_OK) {
		std::cerr << "UncompressXml: Failed to decompress XML: ";
		std::cerr << zError(status) << std::endl;
		delete[] destination;
		fclose(temp);
		return false;
	}

	fwrite(destination, 1, destLength, temp);
	delete[] destination;

	bool result = document.LoadFile(temp);

	fclose(temp);

	return result;
}


popen_streambuf::popen_streambuf()
	:
	fFile(NULL),
	fBuffer(NULL)
{
}


popen_streambuf::popen_streambuf(const char* fileName, const char* mode)
	:
	fFile(NULL),
	fBuffer(NULL)
{
	popen_streambuf* buf = open(fileName, mode);
	if (buf == NULL)
		throw -1;
}


popen_streambuf::~popen_streambuf()
{
	close();
}


popen_streambuf*
popen_streambuf::open(const char* fileName, const char* mode)
{
	fFile = popen(fileName, mode);
	if (fFile == NULL)
		return NULL;

	fBuffer = new char[512];
	if (fBuffer == NULL)
		return NULL;

	setg(fBuffer, fBuffer, fBuffer);
	return this;
}


void
popen_streambuf::close()
{
	if (fFile != NULL) {
		pclose(fFile);
		fFile = NULL;
	}

	delete[] fBuffer;
}


std::streamsize
popen_streambuf::xsgetn(char_type* ptr, std::streamsize num)
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


popen_streambuf::traits_type::int_type
popen_streambuf::underflow()
{
	if (gptr() == 0)
		return traits_type::eof();

	if (gptr() < egptr())
		return traits_type::to_int_type(*gptr());

	size_t len = fread(eback(), sizeof(char_type), 512, fFile);
	setg(eback(), eback(), eback() + sizeof(char_type) * len);
	if (len == 0)
		return traits_type::eof();
	return traits_type::to_int_type(*gptr());
}


std::streamsize
popen_streambuf::showmanyc()
{
	if (gptr() == 0)
		return 0;
	if (gptr() < egptr())
		return egptr() - gptr();
	return 0;
}
