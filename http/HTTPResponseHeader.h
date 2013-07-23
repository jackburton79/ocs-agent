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
	~HTTPResponseHeader();
};

#endif /* HTTPRESPONSEHEADER_H_ */
