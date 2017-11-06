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

#include <tinyxml2/tinyxml2.h>


class ElementFinderByName : public tinyxml2::XMLVisitor {
public:
	ElementFinderByName(std::string elementName, bool onlyFullName);
	virtual bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr);

	const tinyxml2::XMLElement* Element() const;
private:
	std::string fElementName;
	const tinyxml2::XMLElement* fElement;
	bool fFull;
};


class ElementFinderByAttribute : public tinyxml2::XMLVisitor {
public:
	ElementFinderByAttribute(std::string attributeName,
							std::string attributeValue,
							bool onlyFullValue);
	virtual bool VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr);

	const tinyxml2::XMLElement* Element() const;
private:
	std::string fAttributeName;
	std::string fAttributeValue;
	const tinyxml2::XMLElement* fElement;
	bool fFull;
};


// XML
std::string
XML::ToString(const tinyxml2::XMLDocument& document)
{
	tinyxml2::XMLPrinter memoryPrinter;
	document.Print(&memoryPrinter);
	std::string str = memoryPrinter.CStr();
	return str;
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
	size_t destLength = 32768;
	char* destination = new char[destLength];
	
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
XML::GetElementText(const tinyxml2::XMLNode& node, std::string elementName)
{
	std::string result;
	const tinyxml2::XMLElement* element = GetElementByName(node, elementName);
	if (element != NULL)
		result = element->GetText();
	return result;
}


const tinyxml2::XMLElement*
XML::GetElementByName(const tinyxml2::XMLNode& node,
						std::string elementName,
						bool onlyFullName)
{
	ElementFinderByName textFinder(elementName, onlyFullName);
	node.Accept(&textFinder);

	return textFinder.Element();
}


const tinyxml2::XMLElement*
XML::GetElementByAttribute(const tinyxml2::XMLNode& node,
							std::string attributeName,
							std::string attributeValue,
							bool onlyFullValue)
{
	ElementFinderByAttribute attributeFinder(attributeName, attributeValue, onlyFullValue);
	node.Accept(&attributeFinder);

	return attributeFinder.Element();
}


// ElementFinderByName
ElementFinderByName::ElementFinderByName(std::string elementName, bool onlyFullName)
	:
	XMLVisitor(),
	fElementName(elementName),
	fElement(NULL),
	fFull(onlyFullName)
{
}


/* virtual */
bool
ElementFinderByName::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute*)
{
	if (fElement != NULL)
		return false;

	if (fFull) {
		if (fElementName.compare(element.Name()) == 0) {
			fElement = &element;
			return false;
		}
	} else {
		if (fElementName.compare(0, fElementName.length(), element.Name(), fElementName.length()) == 0) {
			fElement = &element;
			return false;
		}
	}

	return true;
}



const tinyxml2::XMLElement*
ElementFinderByName::Element() const
{
	return fElement;
}


// ElementFinderByAttribute
ElementFinderByAttribute::ElementFinderByAttribute(std::string attributeName,
											std::string attributeValue,
											bool onlyFullValue)
	:
	XMLVisitor(),
	fAttributeName(attributeName),
	fAttributeValue(attributeValue),
	fElement(NULL),
	fFull(onlyFullValue)
{
}


/* virtual */
bool
ElementFinderByAttribute::VisitEnter(const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* attr)
{
	if (fElement != NULL)
		return false;

	if (attr == NULL)
		return true;

	const tinyxml2::XMLAttribute* next = attr;
	while (next != NULL) {
		if (fFull) {
			if (fAttributeName.compare(next->Name()) == 0
					&& fAttributeValue.compare(next->Value()) == 0) {
				fElement = &element;
				return false;
			}
		} else {
			if (fAttributeName.compare(next->Name()) == 0
					&& fAttributeValue.compare(0, fAttributeValue.length(),
						next->Value(), fAttributeValue.length()) == 0) {
				fElement = &element;
				return false;
			}
		}
		next = next->Next();
	}

	return true;
}



const tinyxml2::XMLElement*
ElementFinderByAttribute::Element() const
{
	return fElement;
}
