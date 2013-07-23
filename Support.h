/*
 * Support.h
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <streambuf>
#include <string>

class TiXmlDocument;
bool CompressXml(const TiXmlDocument& document, char*& destination, size_t& destLength);
bool UncompressXml(const char* source, size_t sourceLen, TiXmlDocument& document);

class popen_streambuf : public std::streambuf {
public:
	popen_streambuf();
	popen_streambuf(const char* fileName, const char* mode);
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


static inline std::string& ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

static inline std::string& rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

static inline std::string& trim(std::string& s) {
	return ltrim(rtrim(s));
}

static inline std::string int_to_string(int i) {
	std::ostringstream stream;
	stream << i;
	return stream.str();
}

#endif /* SUPPORT_H_ */
