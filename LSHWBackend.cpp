/*
 * LSHWBackend.cpp
 *
 *  Created on: 22 mar 2021
 *      Author: Stefano Ceccherini
 */

#include "LSHWBackend.h"

#include "Machine.h"
#include "ProcReader.h"
#include "Support.h"

#include <tinyxml2/tinyxml2.h>

#include <XML.h>

LSHWBackend::LSHWBackend()
{
}


LSHWBackend::~LSHWBackend()
{
}


/* virtual */
int
LSHWBackend::Run()
{
	if (!CommandExists("lshw"))
		return false;

	// Load command output into "string"
	CommandStreamBuffer lshw("lshw -xml", "r");
	std::istream iStream(&lshw);
	std::istreambuf_iterator<char> eos;
	std::string string(std::istreambuf_iterator<char>(iStream), eos);

	tinyxml2::XMLDocument doc;
	if (doc.Parse(string.c_str(), string.size()) != tinyxml2::XML_SUCCESS)
		return false;

	const tinyxml2::XMLElement* element = NULL;
	const tinyxml2::XMLElement* tmpElement = NULL;
	element = XML::GetElementByAttribute(doc, "id", "firmware");
	if (element != NULL) {
		Component biosInfo;
		biosInfo.fields["release_date"] = XML::GetFirstChildElementText(element, "date");
		biosInfo.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		biosInfo.fields["version"] = XML::GetFirstChildElementText(element, "version");
		gComponents["BIOS"].MergeWith(biosInfo);
	}

	element = XML::GetElementByAttribute(doc, "class", "system");
	if (element != NULL) {
		Component systemInfo;
		systemInfo.fields["name"] = XML::GetFirstChildElementText(element, "product");
		systemInfo.fields["version"] = XML::GetFirstChildElementText(element, "version");
		systemInfo.fields["serial"] = XML::GetFirstChildElementText(element, "serial");
		systemInfo.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		gComponents["SYSTEM"].MergeWith(systemInfo);

		Component chassisInfo;
		// TODO: Check if this is always correct
		chassisInfo.fields["type"] = XML::GetFirstChildElementText(element, "description");
		gComponents["CHASSIS"].MergeWith(chassisInfo);
	}

	element = XML::GetElementByAttribute(doc, "id", "core");
	if (element != NULL) {
		Component boardInfo;
		boardInfo.fields["name"] = XML::GetFirstChildElementText(element, "product");
		boardInfo.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		boardInfo.fields["serial"] = XML::GetFirstChildElementText(element, "serial");
		gComponents["BOARD"].MergeWith(boardInfo);
	}

	element = XML::GetElementByAttribute(doc, "id", "display");
	if (element != NULL) {
		// TODO: there could be multiple displays
		Component info;
		info.fields["name"] = XML::GetFirstChildElementText(element, "description");
		info.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		info.fields["type"] = XML::GetFirstChildElementText(element, "product");
		gComponents["GRAPHICS"].MergeWith(info);
	}

	/*if (fMemoryInfo.size() == 0) {
		const size_t memoryLength = ::strlen("memory");
		element = XML::GetElementByAttribute(doc, "id", "memory", XML::match_partial);
		while (element != NULL) {
			std::string memoryCaption;
			tmpElement = element->FirstChildElement("description");
			if (tmpElement != NULL)
				memoryCaption = tmpElement->GetText();
			const tinyxml2::XMLElement* bankElement
				= XML::GetElementByAttribute(*element, "id", "bank", XML::match_partial);
			if (bankElement == NULL) {
				// In some cases (VMs for example), there is no "bank" element
				memory_device_info info;
				info.caption = memoryCaption;
				info.purpose = info.caption;
				tmpElement = element->FirstChildElement("size");
				if (tmpElement != NULL) {
					memory_device_info info;
					info.caption = memoryCaption;
					info.purpose = info.caption;
					info.size = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
					fMemoryInfo.push_back(info);
				}
			} else {
				while (bankElement != NULL) {
					memory_device_info info;
					info.caption = memoryCaption;
					info.purpose = info.caption;
					info.description = XML::GetFirstChildElementText(bankElement, "description");
					info.type = RAM_type_from_description(info.description);
					info.serial = XML::GetFirstChildElementText(bankElement, "serial");

					tmpElement = bankElement->FirstChildElement("clock");
					if (tmpElement != NULL) {
						// In Hz, usually, but we should check the unit
						info.speed = ::strtoul(tmpElement->GetText(), NULL, 10) / (1000 * 1000);
					}
					tmpElement = bankElement->FirstChildElement("size");
					if (tmpElement != NULL) {
						info.size = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
						fMemoryInfo.push_back(info);
					}
					bankElement = bankElement->NextSiblingElement();
				}
			}

			element = element->NextSiblingElement();
			if (element != NULL) {
				if (::strncasecmp(element->Attribute("id"), "memory", memoryLength) != 0)
					break;
			}
		}
	}*/

	return 0;
}
