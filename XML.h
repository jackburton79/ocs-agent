/*
 * XML.h
 *
 *  Created on: 01/sep/2014
 *      Author: Stefano Ceccherini
 */

#ifndef XML_H_
#define XML_H_

#include <string>

namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
	class XMLNode;
};

class XML {
public:
	static std::string ToString(const tinyxml2::XMLDocument& document);
	static bool Compress(const tinyxml2::XMLDocument& document, char*& destination, size_t& destLength);
	static bool Uncompress(const char* source, size_t sourceLen, tinyxml2::XMLDocument& document);

	static std::string GetElementText(const tinyxml2::XMLNode& node, const std::string& elementName);

	static const tinyxml2::XMLElement* GetElementByName(const tinyxml2::XMLNode& node,
														std::string elementName, bool onlyFullName = true);
														
	static const tinyxml2::XMLElement* GetElementByAttribute(const tinyxml2::XMLNode& node,
													std::string attributeName,
													std::string attributeValue,
													bool onlyFullValue = true);
};

#endif /* XML_H_ */
