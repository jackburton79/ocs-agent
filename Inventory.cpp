/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Inventory.h"
#include "Machine.h"
#include "RunningProcessesList.h"
#include "Support.h"

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

	_AddAccountInfo(content);

	request->LinkEndChild(content);

	_AddBIOSInfo(content);
	_AddCPUsInfo(content);
	_AddDrivesInfo(content);
	_AddHardwareInfo(content);
	_AddNetworksInfo(content);
	_AddProcessesInfo(content);
	_AddSoftwaresInfo(content);
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


// Private
void
Inventory::_AddAccountInfo(TiXmlElement* parent)
{
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

	parent->LinkEndChild(accountInfo);
}


void
Inventory::_AddBIOSInfo(TiXmlElement* parent)
{
	TiXmlElement* bios = new TiXmlElement("BIOS");

	TiXmlElement* assettag = new TiXmlElement("ASSETTAG");
	assettag->LinkEndChild(new TiXmlText(fMachine->AssetTag()));

	TiXmlElement* bdate = new TiXmlElement("BDATE");
	bdate->LinkEndChild(new TiXmlText(fMachine->BIOSDate()));

	TiXmlElement* bmanufacturer = new TiXmlElement("BMANUFACTURER");
	bmanufacturer->LinkEndChild(new TiXmlText(fMachine->BIOSManufacturer()));

	TiXmlElement* bversion = new TiXmlElement("BVERSION");
	bversion->LinkEndChild(new TiXmlText(fMachine->BIOSVersion()));

	TiXmlElement* mmanufacturer = new TiXmlElement("MMANUFACTURER");
	mmanufacturer->LinkEndChild(new TiXmlText(fMachine->MachineManufacturer()));

	TiXmlElement* systemModel = new TiXmlElement("SMODEL");
	systemModel->LinkEndChild(new TiXmlText(fMachine->SystemModel()));

	TiXmlElement* ssn = new TiXmlElement("SSN");
	ssn->LinkEndChild(new TiXmlText(fMachine->SystemSerialNumber()));

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
				new TiXmlText(fMachine->ProcessorManufacturer(i)));
		serial->LinkEndChild(
				new TiXmlText(fMachine->ProcessorSerialNumber(i)));
		speed->LinkEndChild(
				new TiXmlText(fMachine->ProcessorSpeed(i)));
		model->LinkEndChild(
				new TiXmlText(fMachine->ProcessorType(i)));

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
	TiXmlElement* hardware = new TiXmlElement("HARDWARE");

	TiXmlElement* checksum = new TiXmlElement("CHECKSUM");
	checksum->LinkEndChild(new TiXmlText("262143"));
	// TODO: Calculate and add checksum
    //<CHECKSUM>262143</CHECKSUM>

    TiXmlElement* dateLastLoggedUser = new TiXmlElement("DATELASTLOGGEDUSER");
    dateLastLoggedUser->LinkEndChild(new TiXmlText("Thu Jul 11 13:24"));
    //<DATELASTLOGGEDUSER>Thu Jul 11 13:24</DATELASTLOGGEDUSER>

    TiXmlElement* defaultGW = new TiXmlElement("DEFAULTGATEWAY");
    defaultGW->LinkEndChild(new TiXmlText("192.168.0.1"));
    //<DEFAULTGATEWAY>192.168.22.9</DEFAULTGATEWAY>

    TiXmlElement* description = new TiXmlElement("DESCRIPTION");
    description->LinkEndChild(new TiXmlText("i686/00-01-23 22:09:06"));
   // <DESCRIPTION>i686/00-01-23 22:09:06</DESCRIPTION>

    TiXmlElement* dns = new TiXmlElement("DNS");
    dns->LinkEndChild(new TiXmlText("192.168.0.1"));
    //<DNS>192.168.22.23/192.168.22.24</DNS>

    TiXmlElement* ipAddress = new TiXmlElement("IPADDR");
    ipAddress->LinkEndChild(new TiXmlText("192.168.0.1"));
    //<IPADDR>192.168.22.33</IPADDR>

    TiXmlElement* lastLoggedUser = new TiXmlElement("LASTLOGGEDUSER");
    lastLoggedUser->LinkEndChild(new TiXmlText("root"));
   // <LASTLOGGEDUSER>root</LASTLOGGEDUSER>

    TiXmlElement* memory = new TiXmlElement("MEMORY");
    memory->LinkEndChild(new TiXmlText(fMachine->KernelInfo().memory));
    //<MEMORY>521</MEMORY>

    TiXmlElement* name = new TiXmlElement("NAME");
    name->LinkEndChild(new TiXmlText(fMachine->KernelInfo().hostname));

    TiXmlElement* osComments = new TiXmlElement("OSCOMMENTS");
    osComments->LinkEndChild(new TiXmlText(fMachine->KernelInfo().comments));

    TiXmlElement* osName = new TiXmlElement("OSNAME");
    // TODO: Fix this
    osName->LinkEndChild(new TiXmlText("openSUSE 12.1 (i586)"));

    TiXmlElement* osVersion = new TiXmlElement("OSVERSION");
    osVersion->LinkEndChild(new TiXmlText(fMachine->KernelInfo().os_release));

    TiXmlElement* processorN = new TiXmlElement("PROCESSORN");

    processorN->LinkEndChild(new TiXmlText(int_to_string(fMachine->CountProcessors())));

    TiXmlElement* processorS = new TiXmlElement("PROCESSORS");
    processorS->LinkEndChild(new TiXmlText(fMachine->ProcessorSpeed(0)));

    TiXmlElement* processorT = new TiXmlElement("PROCESSORT");
    processorT->LinkEndChild(new TiXmlText(fMachine->ProcessorType(0)));

    TiXmlElement* swap = new TiXmlElement("SWAP");
    swap->LinkEndChild(new TiXmlText(fMachine->KernelInfo().swap));
    //<SWAP>1669</SWAP>

    TiXmlElement* userID = new TiXmlElement("USERID");
    // TODO: Fix this
    userID->LinkEndChild(new TiXmlText("root"));
    //<USERID>root</USERID>

    TiXmlElement* uuid = new TiXmlElement("UUID");
    uuid->LinkEndChild(new TiXmlText(""));
    //<UUID></UUID>

    TiXmlElement* vmSystem = new TiXmlElement("VMSYSTEM");
    vmSystem->LinkEndChild(new TiXmlText("Physical"));
   // <VMSYSTEM>Xen</VMSYSTEM>

    TiXmlElement* workGroup = new TiXmlElement("WORKGROUP");
    workGroup->LinkEndChild(new TiXmlText(fMachine->KernelInfo().domain_name));


    hardware->LinkEndChild(checksum);
    hardware->LinkEndChild(dateLastLoggedUser);
    hardware->LinkEndChild(defaultGW);
    hardware->LinkEndChild(description);
    hardware->LinkEndChild(dns);
    hardware->LinkEndChild(ipAddress);
    hardware->LinkEndChild(lastLoggedUser);
    hardware->LinkEndChild(memory);
    hardware->LinkEndChild(name);
    hardware->LinkEndChild(osComments);
    hardware->LinkEndChild(osName);
    hardware->LinkEndChild(osVersion);
    hardware->LinkEndChild(processorN);
    hardware->LinkEndChild(processorS);
    hardware->LinkEndChild(processorT);
    hardware->LinkEndChild(swap);
    hardware->LinkEndChild(userID);
    hardware->LinkEndChild(uuid);
    hardware->LinkEndChild(vmSystem);
    hardware->LinkEndChild(workGroup);
    parent->LinkEndChild(hardware);
}


void
Inventory::_AddNetworksInfo(TiXmlElement* parent)
{

}


void
Inventory::_AddProcessesInfo(TiXmlElement* parent)
{
	RunningProcessesList processList;
	process_info processInfo;
	while (processList.GetNext(processInfo)) {
		TiXmlElement* process = new TiXmlElement("PROCESSES");

		TiXmlElement* cmd = new TiXmlElement("CMD");
		cmd->LinkEndChild(new TiXmlText(processInfo.cmdline));

		TiXmlElement* cpuUsage = new TiXmlElement("CPUUSAGE");
		TiXmlElement* mem = new TiXmlElement("MEM");

		TiXmlElement* pid = new TiXmlElement("PID");
		pid->LinkEndChild(new TiXmlText(int_to_string(processInfo.pid)));

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
Inventory::_AddSoftwaresInfo(TiXmlElement* parent)
{

}


void
Inventory::_AddUsersInfo(TiXmlElement* parent)
{
	TiXmlElement* users = new TiXmlElement("USERS");

	for (int i = 0; i < fMachine->Users().Count(); i++) {
		TiXmlElement* login = new TiXmlElement("LOGIN");
		login->LinkEndChild(new TiXmlText(fMachine->Users().UserAt(i).c_str()));
		users->LinkEndChild(login);
	}
	parent->LinkEndChild(users);
}
