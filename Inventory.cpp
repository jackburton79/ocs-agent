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
{
	// TODO Auto-generated constructor stub

}

Inventory::~Inventory()
{
	// TODO Auto-generated destructor stub
}


void
Inventory::Build()
{
	Machine machine;
	// TODO: Finish this, cleanup.

	TiXmlDocument document;
	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "UTF-8", "");
	TiXmlElement* request = new TiXmlElement("REQUEST");
	document.LinkEndChild(declaration);
	document.LinkEndChild(request);

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

	TiXmlElement* deviceId = new TiXmlElement("DEVICEID");
	request->LinkEndChild(deviceId);

	TiXmlElement* query = new TiXmlElement("QUERY");
	request->LinkEndChild(query);

	if (!document.SaveFile("test.xml"))
		std::cerr << "Cannot create output file." << std::endl;
}


void
Inventory::Print()
{

}


void
Inventory::Send()
{

}
