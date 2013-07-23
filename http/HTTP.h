/*
 * HTTP.h
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <string>

class HTTPRequestHeader;
class HTTPResponseHeader;
class HTTP {
public:
	HTTP();
	HTTP(const std::string hostName, int port = 80);
	~HTTP();

	void Close();
	void ClearPendingRequests();
	HTTPRequestHeader CurrentRequest() const;
	int Error() const;
	std::string ErrorString() const;

	int Get(const std::string path);
	HTTPResponseHeader LastResponse() const;
	int Post(const std::string path, char* data);

	int SetHost(const std::string hostName, int port = 80);

};

#endif /* HTTP_H_ */
