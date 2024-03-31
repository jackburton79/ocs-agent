/*
 * HTTPRequestHeader.h
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2024 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef HTTPREQUESTHEADER_H_
#define HTTPREQUESTHEADER_H_

#include <string>

#include "HTTPHeader.h"

class HTTPRequestHeader : public HTTPHeader {
public:
	HTTPRequestHeader();
	HTTPRequestHeader(const HTTPRequestHeader&);
	HTTPRequestHeader(const std::string& method, const std::string& path,
			int majorVer = 1, int minorVer = 1);
	~HTTPRequestHeader();

	std::string UserAgent() const;
	void SetUserAgent(const std::string& agent);

	std::string Method() const;
	std::string URL() const;

	void SetAuthentication(int type, std::string userName, std::string password);

	virtual std::string ToString() const;

	void SetRequest(const std::string& method, const std::string& path,
			int majorVer = 1, int minorVer = 1);

	virtual void Clear();
	
	HTTPRequestHeader& operator=(const HTTPRequestHeader& header);

private:
	void _Init();

	std::string fMethod;
	std::string fURL;
	std::string fUserAgent;
};

#endif /* HTTPREQUESTHEADER_H_ */
