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

	// TODO: ??? We can't store anything
	for (int a = 0; a < 1; a++) {
		TiXmlElement* keyName = new TiXmlElement("KEYNAME");
		keyName->LinkEndChild(new TiXmlText("TAG"));

		TiXmlElement* keyValue = new TiXmlElement("KEYVALUE");
		keyValue->LinkEndChild(new TiXmlText("NA"));

		accountInfo->LinkEndChild(keyName);
		accountInfo->LinkEndChild(keyValue);
	}

	content->LinkEndChild(accountInfo);
	request->LinkEndChild(content);

	TiXmlElement* bios = new TiXmlElement("BIOS");

	TiXmlElement* assettag = new TiXmlElement("ASSETTAG");
	assettag->LinkEndChild(new TiXmlText(machine.AssetTag().c_str()));

	TiXmlElement* bdate = new TiXmlElement("BDATE");
	bdate->LinkEndChild(new TiXmlText(machine.BIOSDate().c_str()));

	TiXmlElement* bmanufacturer = new TiXmlElement("BMANUFACTURER");
	bmanufacturer->LinkEndChild(new TiXmlText(machine.BIOSManufacturer().c_str()));

	TiXmlElement* bversion = new TiXmlElement("BVERSION");
	bversion->LinkEndChild(new TiXmlText(machine.BIOSVersion().c_str()));

	TiXmlElement* mmanufacturer = new TiXmlElement("MMANUFACTURER");
	mmanufacturer->LinkEndChild(new TiXmlText(machine.MachineManufacturer().c_str()));

	TiXmlElement* systemModel = new TiXmlElement("SMODEL");
	systemModel->LinkEndChild(new TiXmlText(machine.SystemModel().c_str()));

	TiXmlElement* ssn = new TiXmlElement("SSN");
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
		TiXmlElement* cpu = new TiXmlElement("CPUS");
		TiXmlElement* manufacturer = new TiXmlElement("MANUFACTURER");
		TiXmlElement* serial = new TiXmlElement("SERIAL");
		TiXmlElement* speed = new TiXmlElement("SPEED");
		TiXmlElement* model = new TiXmlElement("TYPE");

		// TODO: Seems like we should interpred the vendor_id
		manufacturer->LinkEndChild(
				new TiXmlText(machine.ProcessorManufacturer(i).c_str()));
		serial->LinkEndChild(
				new TiXmlText(machine.ProcessorSerialNumber(i).c_str()));
		speed->LinkEndChild(
				new TiXmlText(machine.ProcessorSpeed(i).c_str()));
		model->LinkEndChild(
				new TiXmlText(machine.ProcessorType(i).c_str()));

		cpu->LinkEndChild(model);
		cpu->LinkEndChild(manufacturer);
		cpu->LinkEndChild(serial);
		cpu->LinkEndChild(speed);

		content->LinkEndChild(cpu);
	}

	TiXmlElement* deviceId = new TiXmlElement("DEVICEID");
	deviceId->LinkEndChild(new TiXmlText(deviceID));

	request->LinkEndChild(deviceId);

	TiXmlElement* query = new TiXmlElement("QUERY");

	// TODO: We only do Inventory for now
	query->LinkEndChild(new TiXmlText("INVENTORY"));
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
