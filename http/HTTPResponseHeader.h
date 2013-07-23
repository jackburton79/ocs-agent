/*
 * HTTPResponseHeader.h
 *
 *  Created on: 23/lug/2013
 *      Author: stefano
 */

#ifndef HTTPRESPONSEHEADER_H_
#define HTTPRESPONSEHEADER_H_

#include "HTTPHeader.h"

class HTTPResponseHeader: public HTTPHeader {
public:
	HTTPResponseHeader();
	HTTPResponseHeader(int code, const std::string text = "",
			const int majVersion = 1, const int minVersion = 1);
	HTTPResponseHeader(const HTTPResponseHeader& header);
	~HTTPResponseHeader();

	std::string ReasonPhrase() const;
	void SetStatusLine(int code, const std::string text = "",
			const int majVersion = 1, const int minVersion = 1);
	int StatusCode() const;

	virtual std::string ToString() const;

	HTTPResponseHeader& operator=(const HTTPResponseHeader& header);

private:
	std::string fText;
	int fCode;
};

#endif /* HTTPRESPONSEHEADER_H_ */
