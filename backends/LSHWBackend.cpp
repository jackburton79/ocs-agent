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

#include <iostream>

#include <XML.h>


LSHWBackend::LSHWBackend()
{
}


LSHWBackend::~LSHWBackend()
{
}


/* virtual */
bool
LSHWBackend::IsAvailable() const
{
	if (!CommandExists("lshw"))
		return false;
	return true;
}


void
LSHWBackend::_GetCPUInfo(const tinyxml2::XMLDocument& doc)
{
	// TODO: multiple cpus
	const tinyxml2::XMLElement* element = XML::GetElementByAttribute(doc, "class", "processor");
	if (element != NULL) {
		Component cpuInfo;
		cpuInfo.fields["vendor"] = XML::GetFirstChildElementText(element,
																	"vendor");
		// In Hz, usually, but we should check the unit
		// we report it in MHZ
		std::string CPUSpeedString = XML::GetFirstChildElementText(element,
																	"capacity");
		unsigned int CPUSpeedNumber = ::strtoul(CPUSpeedString.c_str(), NULL, 0)
				/ (1000 * 1000);
		if (CPUSpeedNumber > 0)
			cpuInfo.fields["speed"] = uint_to_string(CPUSpeedNumber);

		std::string CPUCurrentSpeedString = XML::GetFirstChildElementText(
				element, "size");
		unsigned int CPUCurrentSpeedNumber = ::strtoul(
				CPUCurrentSpeedString.c_str(), NULL, 0) / (1000 * 1000);
		if (CPUCurrentSpeedNumber > 0)
			cpuInfo.fields["current_speed"] = uint_to_string(
					CPUCurrentSpeedNumber);

		cpuInfo.fields["type"] = XML::GetFirstChildElementText(element,
																"product");
		cpuInfo.fields["serial"] = XML::GetFirstChildElementText(element,
																	"serial");
		std::string dataWidthString = XML::GetFirstChildElementText(element,
																	"width");
		std::string dataWidth = uint_to_string(
				::strtoul(dataWidthString.c_str(), NULL, 0));
		cpuInfo.fields["width"] = dataWidth;

		const tinyxml2::XMLElement* cfgElement = element->FirstChildElement("configuration");
		if (cfgElement != NULL) {
			const tinyxml2::XMLElement* coresElement
						= XML::GetElementByAttribute(*cfgElement, "id", "cores", XML::match_full);
			if (coresElement != NULL)
				cpuInfo.fields["cores"] = coresElement->FindAttribute("value")->Value();
			else {
				// There is no "cores" field, let's assume there is only one core
				cpuInfo.fields["cores"] = "1";
			}
		}

		gComponents.Merge("CPU", cpuInfo);
	}
}

/* virtual */
int
LSHWBackend::Run()
{
	if (!IsAvailable())
		return -1;

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
		gComponents.Merge("BIOS", biosInfo);
	}

	element = XML::GetElementByAttribute(doc, "class", "system");
	if (element != NULL) {
		Component systemInfo;
		systemInfo.fields["name"] = XML::GetFirstChildElementText(element, "product");
		systemInfo.fields["version"] = XML::GetFirstChildElementText(element, "version");
		systemInfo.fields["serial"] = XML::GetFirstChildElementText(element, "serial");
		systemInfo.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		gComponents.Merge("SYSTEM", systemInfo);

		Component chassisInfo;
		// TODO: Check if this is always correct
		chassisInfo.fields["type"] = XML::GetFirstChildElementText(element, "description");
		gComponents.Merge("CHASSIS", chassisInfo);
	}

	element = XML::GetElementByAttribute(doc, "id", "core");
	if (element != NULL) {
		Component boardInfo;
		boardInfo.fields["name"] = XML::GetFirstChildElementText(element, "product");
		boardInfo.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		boardInfo.fields["serial"] = XML::GetFirstChildElementText(element, "serial");
		gComponents.Merge("BOARD", boardInfo);
	}

	// TODO: multiple cpus
	_GetCPUInfo(doc);

	element = XML::GetElementByAttribute(doc, "id", "display");
	if (element != NULL) {
		// TODO: there could be multiple displays
		Component info;
		info.fields["name"] = XML::GetFirstChildElementText(element, "description");
		info.fields["vendor"] = XML::GetFirstChildElementText(element, "vendor");
		info.fields["type"] = XML::GetFirstChildElementText(element, "product");
		gComponents.Merge("GRAPHICS", info);
	}

	int slotNum = 0;
	std::ostringstream s;
	s << "MEMORY" << slotNum;
	components_map::iterator ramSlotIterator = gComponents.find(s.str());
	// already some slot info, bail out: from experience, LSHW's output
	// for ram slots is less nice than dmidecode's
	if (ramSlotIterator != gComponents.end())
		return 0;

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
			Component ramSlot;
			// In some cases (VMs for example), there is no "bank" element
			ramSlot.fields["caption"] = memoryCaption;
			ramSlot.fields["purpose"] = ramSlot.fields["caption"];
			tmpElement = element->FirstChildElement("size");
			if (tmpElement != NULL) {
				ramSlot.fields["size"] = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
				std::ostringstream s;
				s << "MEMORY" << slotNum;
				gComponents.Merge(s.str().c_str(), ramSlot);
				slotNum++;
			}
		} else {
			while (bankElement != NULL) {
				Component ramSlot;
				ramSlot.fields["caption"] = memoryCaption;
				ramSlot.fields["purpose"] = memoryCaption;
				ramSlot.fields["description"] = XML::GetFirstChildElementText(bankElement, "description");
				ramSlot.fields["type"] = RAM_type_from_description(ramSlot.fields["description"]);
				ramSlot.fields["serial"] = XML::GetFirstChildElementText(bankElement, "serial");

				tmpElement = bankElement->FirstChildElement("clock");
				if (tmpElement != NULL) {
					// In Hz, usually, but we should check the unit
					unsigned long ramSpeed = ::strtoul(tmpElement->GetText(), NULL, 10) / (1000 * 1000);
					ramSlot.fields["speed"] = uint_to_string(ramSpeed);
				}
				tmpElement = bankElement->FirstChildElement("size");
				if (tmpElement != NULL) {
					unsigned long ramSize = ::strtoul(tmpElement->GetText(), NULL, 10) / (1024 * 1024);
					if (ramSize == 0)
						ramSlot.fields["type"] = "Empty slot";
					ramSlot.fields["size"] = uint_to_string(ramSize);
					std::ostringstream s;
					s << "MEMORY" << slotNum;
					gComponents.Merge(s.str().c_str(), ramSlot);
					slotNum++;
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

	return 0;
}
