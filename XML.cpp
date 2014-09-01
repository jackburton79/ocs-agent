/*
 * XML.cpp
 *
 *  Created on: 01/ago/2014
 *      Author: Stefano Ceccherini
 */


#include "Support.h"
#include "XML.h"

#include <iostream>
#include <streambuf>
#include <string>
#include <string.h>

#include <zlib.h>

class ElementFinder : public tinyxml2::XMLVisitor {
public:
	ElementFinder(std::string elementName);
	virtual bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr);

	std::string Response() const;
private:
	std::string fElementName;
	std::string fResponse;
};


ElementFinder::ElementFinder(std::string elementName)
	:
	XMLVisitor(),
	fElementName(elementName),
	fResponse("")
{
}


/* virtual */
bool
ElementFinder::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute*)
{
	if (fElementName.compare(element.Name()) == 0) {
		fResponse = element.GetText();
		return false;
	}
	
	return true;
}



std::string
ElementFinder::Response() const
{
	return fResponse;
}



bool
XML::Compress(const tinyxml2::XMLDocument& document, char*& destination, size_t& destLength)
{
	tinyxml2::XMLPrinter memoryPrinter;
	document.Print(&memoryPrinter);

	int fileSize = memoryPrinter.CStrSize() - 1;

	destLength = compressBound(fileSize);
	destination = new char[destLength];

	if (int compressStatus = compress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)memoryPrinter.CStr(), (uLong)fileSize) != Z_OK) {
		std::cerr << "Compress returned error: " << zError(compressStatus) << std::endl;
		delete[] destination;
		return false;
	}

	return true;
}


bool
XML::Uncompress(const char* source, size_t sourceLen, tinyxml2::XMLDocument& document)
{
	char* destination = new char[32768];
	size_t destLength = 32768;

	if (int status = uncompress((Bytef*)destination, (uLongf*)&destLength,
			(const Bytef*)source, (uLong)sourceLen) != Z_OK) {
		std::cerr << "UncompressXml: Failed to decompress XML: ";
		std::cerr << zError(status) << std::endl;
		delete[] destination;
		return false;
	}

	tinyxml2::XMLError result = document.Parse(destination, destLength - 1);

	delete[] destination;

	return result == tinyxml2::XML_SUCCESS;
}


std::string
XML::GetTextElementValue(const tinyxml2::XMLDocument& document, std::string elementName)
{
	ElementFinder responseFinder(elementName);
        document.Accept(&responseFinder);

	return responseFinder.Response();
}

