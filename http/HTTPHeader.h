/*
 * HTTPHeader.h
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2024 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef HTTPHEADER_H_
#define HTTPHEADER_H_

#include <map>
#include <string>

class ICompareString {
public:
	bool operator()(const std::string& a, const std::string& b) const;
};

class HTTPHeader {
public:
	HTTPHeader();
	HTTPHeader(const std::string& string);
	HTTPHeader(const HTTPHeader&);
	virtual ~HTTPHeader();

	size_t ContentLength() const;
	bool HasContentLength() const;
	void SetContentLength(size_t len);
	
	std::string ContentType() const;
	bool HasContentType() const;
	void SetContentType(const std::string& type);

	std::string Value(const std::string& key) const;
	bool HasKey(const std::string& key) const;
	void SetValue(const std::string& key, const std::string& value);

	virtual std::string ToString() const;

	virtual void Clear();

	HTTPHeader& operator=(const HTTPHeader&);

protected:
	std::map<std::string, std::string, ICompareString> fValues;
};

#endif /* HTTPHEADER_H_ */
