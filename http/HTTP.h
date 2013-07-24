/*
 * HTTP.h
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <string>

#include "HTTPRequestHeader.h"
#include "HTTPResponseHeader.h"

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
	HTTPResponseHeader LastResponse() const;

	int SetHost(const std::string hostName, int port = 80);
	int Get(const std::string path);
	int Post(const std::string path, char* data);

	int Request(HTTPRequestHeader& header, const void* data);


private:
	std::string _HostFromConnectionString(std::string string) const;
	bool _HandleConnectionIfNeeded(const std::string host, const int port = 80);

	std::string fHost;
	int fPort;
	int fFD;
	int fLastError;

	HTTPRequestHeader fCurrentRequest;
	HTTPResponseHeader fLastResponse;
};

#endif /* HTTP_H_ */
