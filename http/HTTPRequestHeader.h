/*
 * HTTPRequestHeader.h
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#ifndef HTTPREQUESTHEADER_H_
#define HTTPREQUESTHEADER_H_

#include <string>

#include "HTTPHeader.h"

class HTTPRequestHeader : public HTTPHeader {
public:
	HTTPRequestHeader();
	HTTPRequestHeader(const HTTPRequestHeader&);
	HTTPRequestHeader(const std::string method, const std::string path,
			int majorVer = 1, int minorVer = 1);
	~HTTPRequestHeader();

	std::string Method() const;
	std::string Path() const;

	void SetRequest(const std::string method, const std::string path,
			int majorVer = 1, int minorVer = 1);
	HTTPRequestHeader& operator=(const HTTPRequestHeader& header);
};

#endif /* HTTPREQUESTHEADER_H_ */
