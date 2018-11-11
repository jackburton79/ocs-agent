/*
 * ZLibCompressor.h
 *
 *  Created on: 03 mag 2018
 *      Author: Stefano ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef ZLIBCOMPRESSOR_H_
#define ZLIBCOMPRESSOR_H_

#include <sys/types.h>

class ZLibCompressor {
public:
	static bool Compress(const char* source, size_t sourceLength, char*& destination, size_t& destLength);
	static bool Uncompress(const char* source, size_t sourceLen, char*& destination, size_t& destLength);
};



#endif /* ZLIBCOMPRESSOR_H_ */
