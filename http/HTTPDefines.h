/*
 * HTTPDefines.h
 *
 *  Created on: 23/lug/2013
 *  Copyright 2013-2017 Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef HTTPDEFINES_H_
#define HTTPDEFINES_H_

#include <string>

#define CRLF "\015\012"

#define HTTP_OK 200
enum HTTP_AUTH_TYPE {
	HTTP_AUTH_TYPE_BASIC = 0
};

extern std::string HTTPProtocolPrefix;
extern std::string HTTPHost;
extern std::string HTTPContentType;
extern std::string HTTPContentLength;
extern std::string HTTPUserAgent;

#endif /* HTTPDEFINES_H_ */
