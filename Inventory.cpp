/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Inventory.h"
#include "Machine.h"

#include <tinyxml.h>

Inventory::Inventory()
	:
	fDocument(NULL)
{
	fDocument = new TiXmlDocument;
}


Inventory::~Inventory()
{
	delete fDocument;
}


bool
Inventory::Build(const char* deviceID)
{
	Machine machine;
	// TODO: Finish this, cleanup.
	try {
		machine.RetrieveData();
	} catch (...) {
		std::cerr << "Cannot retrieve machine data." << std::endl;
		return false;
	}

	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "UTF-8", "");
	TiXmlElement* request = new TiXmlElement("REQUEST");
	fDocument->LinkEndChild(declaration);
	fDocument->LinkEndChild(request);

	TiXmlElement* content = new TiXmlElement("CONTENT");
	TiXmlElement* accountInfo = new TiXmlElement("ACCOUNTINFO");
	content->LinkEndChild(accountInfo);
	request->LinkEndChild(content);

	TiXmlElement* bios = new TiXmlElement("BIOS");

	TiXmlElement* assettag = new TiXmlElement("ASSETTAG");
	TiXmlElement* bdate = new TiXmlElement("BDATE");
	TiXmlElement* bmanufacturer = new TiXmlElement("BMANUFACTURER");
	TiXmlElement* bversion = new TiXmlElement("BVERSION");
	TiXmlElement* mmanufacturer = new TiXmlElement("MMANUFACTURER");
	TiXmlElement* systemModel = new TiXmlElement("SMODEL");
	TiXmlElement* ssn = new TiXmlElement("SSN");

	bmanufacturer->LinkEndChild(new TiXmlText(machine.BIOSManufacturer().c_str()));
	bversion->LinkEndChild(new TiXmlText(machine.BIOSVersion().c_str()));
	bdate->LinkEndChild(new TiXmlText(machine.BIOSDate().c_str()));

	mmanufacturer->LinkEndChild(new TiXmlText(machine.MachineManufacturer().c_str()));

	systemModel->LinkEndChild(new TiXmlText(machine.SystemModel().c_str()));
	ssn->LinkEndChild(new TiXmlText(machine.SystemSerialNumber().c_str()));

	bios->LinkEndChild(assettag);
	bios->LinkEndChild(bdate);
	bios->LinkEndChild(bmanufacturer);
	bios->LinkEndChild(bversion);
	bios->LinkEndChild(mmanufacturer);
	bios->LinkEndChild(systemModel);
	bios->LinkEndChild(ssn);

	content->LinkEndChild(bios);

	// TODO: Check if the fields name and structure are correct.
	for (int i = 0; i < machine.CountProcessors(); i++) {
		TiXmlElement* cpu = new TiXmlElement("CPU");
		TiXmlElement* model = new TiXmlElement("CPUMODEL");

		cpu->LinkEndChild(model);
		model->LinkEndChild(
				new TiXmlText(machine.ProcessorInfo("model name", i).c_str()));
		content->LinkEndChild(cpu);
	}

	TiXmlElement* deviceId = new TiXmlElement("DEVICEID");
	deviceId->LinkEndChild(new TiXmlText(deviceID));

	request->LinkEndChild(deviceId);

	TiXmlElement* query = new TiXmlElement("QUERY");
	request->LinkEndChild(query);

	return true;
}


bool
Inventory::Save(const char* name)
{
	std::string fullName;
	fullName.append(name).append(".xml");

	return fDocument->SaveFile(fullName.c_str());
}


void
Inventory::Send(const char* serverUrl)
{
	// TODO: Send the inventory
}
