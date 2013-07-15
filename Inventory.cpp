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
	fDocument(NULL),
	fMachine(NULL)
{
	fDocument = new TiXmlDocument;
	fMachine = new Machine;
}


Inventory::~Inventory()
{
	delete fDocument;
	delete fMachine;
}


bool
Inventory::Build(const char* deviceID)
{
	// TODO: Finish this, cleanup.
	try {
		fMachine->RetrieveData();
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

	_AddBIOSInfo(content);
	_AddCPUsInfo(content);
	_AddHardwareInfo(content);
	_AddNetworksInfo(content);
	_AddProcessesInfo(content);
	_AddUsersInfo(content);

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


void
Inventory::_AddBIOSInfo(TiXmlElement* parent)
{
	TiXmlElement* bios = new TiXmlElement("BIOS");

	TiXmlElement* assettag = new TiXmlElement("ASSETTAG");
	assettag->LinkEndChild(new TiXmlText(fMachine->AssetTag().c_str()));

	TiXmlElement* bdate = new TiXmlElement("BDATE");
	bdate->LinkEndChild(new TiXmlText(fMachine->BIOSDate().c_str()));

	TiXmlElement* bmanufacturer = new TiXmlElement("BMANUFACTURER");
	bmanufacturer->LinkEndChild(new TiXmlText(fMachine->BIOSManufacturer().c_str()));

	TiXmlElement* bversion = new TiXmlElement("BVERSION");
	bversion->LinkEndChild(new TiXmlText(fMachine->BIOSVersion().c_str()));

	TiXmlElement* mmanufacturer = new TiXmlElement("MMANUFACTURER");
	mmanufacturer->LinkEndChild(new TiXmlText(fMachine->MachineManufacturer().c_str()));

	TiXmlElement* systemModel = new TiXmlElement("SMODEL");
	systemModel->LinkEndChild(new TiXmlText(fMachine->SystemModel().c_str()));

	TiXmlElement* ssn = new TiXmlElement("SSN");
	ssn->LinkEndChild(new TiXmlText(fMachine->SystemSerialNumber().c_str()));

	bios->LinkEndChild(assettag);
	bios->LinkEndChild(bdate);
	bios->LinkEndChild(bmanufacturer);
	bios->LinkEndChild(bversion);
	bios->LinkEndChild(mmanufacturer);
	bios->LinkEndChild(systemModel);
	bios->LinkEndChild(ssn);

	parent->LinkEndChild(bios);
}


void
Inventory::_AddCPUsInfo(TiXmlElement* parent)
{
	// TODO: Check if the fields name and structure are correct.
	for (int i = 0; i < fMachine->CountProcessors(); i++) {
		TiXmlElement* cpu = new TiXmlElement("CPUS");
		TiXmlElement* manufacturer = new TiXmlElement("MANUFACTURER");
		TiXmlElement* serial = new TiXmlElement("SERIAL");
		TiXmlElement* speed = new TiXmlElement("SPEED");
		TiXmlElement* model = new TiXmlElement("TYPE");

		// TODO: Seems like we should interpred the vendor_id
		manufacturer->LinkEndChild(
				new TiXmlText(fMachine->ProcessorManufacturer(i).c_str()));
		serial->LinkEndChild(
				new TiXmlText(fMachine->ProcessorSerialNumber(i).c_str()));
		speed->LinkEndChild(
				new TiXmlText(fMachine->ProcessorSpeed(i).c_str()));
		model->LinkEndChild(
				new TiXmlText(fMachine->ProcessorType(i).c_str()));

		cpu->LinkEndChild(model);
		cpu->LinkEndChild(manufacturer);
		cpu->LinkEndChild(serial);
		cpu->LinkEndChild(speed);

		parent->LinkEndChild(cpu);
	}
}


void
Inventory::_AddDrivesInfo(TiXmlElement* parent)
{

}


void
Inventory::_AddHardwareInfo(TiXmlElement* parent)
{

}


void
Inventory::_AddNetworksInfo(TiXmlElement* parent)
{

}


void
Inventory::_AddProcessesInfo(TiXmlElement* parent)
{
	// TODO: Get processes from /proc ?
	for (int i = 0; i < fMachine->CountProcesses(); i++) {
		TiXmlElement* process = new TiXmlElement("PROCESSES");

		TiXmlElement* cmd = new TiXmlElement("CMD");
		TiXmlElement* cpuUsage = new TiXmlElement("CPUUSAGE");
		TiXmlElement* mem = new TiXmlElement("MEM");
		TiXmlElement* pid = new TiXmlElement("PID");
		TiXmlElement* started = new TiXmlElement("STARTED");
		TiXmlElement* tty = new TiXmlElement("TTY");
		TiXmlElement* user = new TiXmlElement("USER");
		TiXmlElement* virtualMem = new TiXmlElement("VIRTUALMEMORY");

		process->LinkEndChild(cmd);
		process->LinkEndChild(cpuUsage);
		process->LinkEndChild(mem);
		process->LinkEndChild(pid);
		process->LinkEndChild(started);
		process->LinkEndChild(tty);
		process->LinkEndChild(user);
		process->LinkEndChild(virtualMem);

		parent->LinkEndChild(process);
	}
}


void
Inventory::_AddUsersInfo(TiXmlElement* parent)
{

}
