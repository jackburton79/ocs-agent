/*
 * ZLibCompressor.cpp
 *
 *  Created on: 03 mag 2018
 *      Author: Stefano ceccherini (stefano.ceccherini@gmail.com)
 */

#include "ZLibCompressor.h"

#include <iostream>
#include <zlib.h>

/*static */
bool
ZLibCompressor::Compress(const char* source, size_t sourceLength, char*& destination, size_t& destLength)
{
	destLength = compressBound(sourceLength);
	destination = new char[destLength];

	if (int compressStatus = compress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)source, (uLong)sourceLength) != Z_OK) {
		std::cerr << "Compress returned error: " << zError(compressStatus) << std::endl;
		delete[] destination;
		return false;
	}
	return true;
}


/* static */
bool
ZLibCompressor::Uncompress(const char* source, size_t sourceLen, char*& destination, size_t& destLength)
{
	destLength = 32768;
	destination = new char[destLength];

	if (int status = uncompress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)source, (uLong)sourceLen) != Z_OK) {
		std::cerr << "UncompressXml: Failed to decompress XML: ";
		std::cerr << zError(status) << std::endl;
		delete[] destination;
		return false;
	}

	return true;
}

