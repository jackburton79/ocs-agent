/*
 * HTTPHeader.h
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2014 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef HTTPHEADER_H_
#define HTTPHEADER_H_

#include <map>
#include <string>

class ICompareString {
public:
	bool operator()(const std::string a, const std::string b) const;
};

class HTTPHeader {
public:
	HTTPHeader();
	HTTPHeader(const std::string string);
	HTTPHeader(const HTTPHeader&);
	virtual ~HTTPHeader();

	int ContentLength() const;
	std::string ContentType() const;

	bool HasContentLength() const;
	bool HasContentType() const;
	bool HasKey(const std::string key) const;

	void SetContentLength(int len);
	void SetContentType(const std::string type);
	void SetValue(const std::string key, const std::string value);

	virtual std::string ToString() const;
	std::string Value(const std::string key) const;

	virtual void Clear();

	HTTPHeader& operator=(const HTTPHeader&);

protected:
	std::map<std::string, std::string, ICompareString> fValues;
};

#endif /* HTTPHEADER_H_ */
