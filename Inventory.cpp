/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Configuration.h"
#include "IfConfigReader.h"
#include "Inventory.h"
#include "LoggedUsers.h"
#include "Machine.h"
#include "RunningProcessesList.h"
#include "Support.h"
#include "VolumeReader.h"

#include "http/HTTP.h"

#include <cstdlib>
#include <iostream>
#include <memory>


#include "tinyxml/tinyxml.h"

#define USER_AGENT "OCS-NG_unified_unix_agent_v"

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
	fDocument->Clear();

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


bool
Inventory::Send(const char* serverUrl)
{
	HTTP httpObject;
	httpObject.SetHost(serverUrl);


	// Send Prolog
	TiXmlDocument prolog;
	_WriteProlog(prolog);

	char* prologData = NULL;
	size_t prologLength = 0;
	if (!CompressXml(prolog, prologData, prologLength)) {
		std::cerr << "Error compressing prolog XML" << std::endl;
		return false;
	}

	std::string inventoryUrl(serverUrl);

	HTTPRequestHeader requestHeader;
	requestHeader.SetRequest("POST", inventoryUrl);
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(prologLength);
	requestHeader.SetUserAgent(USER_AGENT);
	if (httpObject.Request(requestHeader, prologData, prologLength) != 0) {
		delete[] prologData;
		std::cerr << "Send: " << httpObject.ErrorString() << std::endl;
		return false;
	}

	delete[] prologData;

	const HTTPResponseHeader& responseHeader = httpObject.LastResponse();
	std::cout << responseHeader.ToString() << std::endl;
	if (responseHeader.StatusCode() == 200
			&& responseHeader.HasContentLength()) {
		size_t contentLength =
				::strtol(responseHeader.Value(HTTPContentLength).c_str(),
						NULL, 10);
		char* resultData = new char[contentLength];
		if (httpObject.Read(resultData, contentLength) < (int)contentLength) {
			delete[] resultData;
			std::cerr << "Failed to read XML response: " << httpObject.ErrorString() << std::endl;
			return false;
		}

		TiXmlDocument document;
		bool uncompress = UncompressXml(resultData, contentLength, document);
		delete[] resultData;

		if (!uncompress) {
			std::cerr << "Inventory::Send(): Failed to decompress XML response" << std::endl;
			return false;
		}

		// TODO: Do something with the reply
	}

	char* compressedData = NULL;
	size_t compressedSize;
	if (!CompressXml(*fDocument, compressedData, compressedSize)) {
		std::cerr << "Error compressing inventory XML" << std::endl;
		return false;
	}

	requestHeader = HTTPRequestHeader();
	requestHeader.SetRequest("POST", inventoryUrl);
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(compressedSize);
	requestHeader.SetUserAgent(USER_AGENT);
	if (httpObject.Request(requestHeader, compressedData, compressedSize) != 0) {
		delete[] compressedData;
		std::cerr << "Send: " << httpObject.ErrorString() << std::endl;
		return false;
	}

	delete[] compressedData;

	return true;
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

	TiXmlElement* mSerial = new TiXmlElement("MSN");
	mSerial->LinkEndChild(new TiXmlText(fMachine->MachineSerialNumber()));

	TiXmlElement* sManufacturer = new TiXmlElement("SMANUFACTURER");
	sManufacturer->LinkEndChild(new TiXmlText(fMachine->SystemManufacturer()));

	TiXmlElement* systemModel = new TiXmlElement("SMODEL");
	systemModel->LinkEndChild(new TiXmlText(fMachine->SystemModel()));

	TiXmlElement* ssn = new TiXmlElement("SSN");
	ssn->LinkEndChild(new TiXmlText(fMachine->SystemSerialNumber()));

	bios->LinkEndChild(assettag);
	bios->LinkEndChild(bdate);
	bios->LinkEndChild(bmanufacturer);
	bios->LinkEndChild(bversion);
	bios->LinkEndChild(mmanufacturer);
	bios->LinkEndChild(mSerial);
	bios->LinkEndChild(sManufacturer);
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

		// TODO: Seems like we should interpretate the vendor_id
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
	VolumeReader reader("-x tmpfs");
	volume_info info;
	while (reader.GetNext(info)) {
		TiXmlElement* drive = new TiXmlElement("DRIVES");

		TiXmlElement* createDate = new TiXmlElement("CREATEDATE");
		createDate->LinkEndChild(new TiXmlText(info.create_date));

		TiXmlElement* fileSystem = new TiXmlElement("FILESYSTEM");
		fileSystem->LinkEndChild(new TiXmlText(info.filesystem));

		TiXmlElement* freeSpace = new TiXmlElement("FREE");
		freeSpace->LinkEndChild(new TiXmlText(int_to_string(info.free)));

		TiXmlElement* label = new TiXmlElement("LABEL");
		label->LinkEndChild(new TiXmlText(info.label));

		TiXmlElement* serial = new TiXmlElement("SERIAL");
		serial->LinkEndChild(new TiXmlText(info.serial));

		TiXmlElement* total = new TiXmlElement("TOTAL");
		total->LinkEndChild(new TiXmlText(int_to_string(info.total)));

		TiXmlElement* type = new TiXmlElement("TYPE");
		type->LinkEndChild(new TiXmlText(info.type));

		TiXmlElement* volumeName = new TiXmlElement("VOLUMN");
		volumeName->LinkEndChild(new TiXmlText(info.name));

		drive->LinkEndChild(createDate);
		drive->LinkEndChild(fileSystem);
		drive->LinkEndChild(freeSpace);
		drive->LinkEndChild(label);
		drive->LinkEndChild(serial);
		drive->LinkEndChild(total);
		drive->LinkEndChild(type);
		drive->LinkEndChild(volumeName);
		parent->LinkEndChild(drive);
	}
}


void
Inventory::_AddHardwareInfo(TiXmlElement* parent)
{
	TiXmlElement* hardware = new TiXmlElement("HARDWARE");

	TiXmlElement* checksum = new TiXmlElement("CHECKSUM");
	checksum->LinkEndChild(new TiXmlText("2"));
	// TODO: Calculate and add checksum
    //<CHECKSUM>262143</CHECKSUM>

    TiXmlElement* dateLastLoggedUser = new TiXmlElement("DATELASTLOGGEDUSER");
    dateLastLoggedUser->LinkEndChild(new TiXmlText("Thu Jul 11 13:24"));
    //<DATELASTLOGGEDUSER>Thu Jul 11 13:24</DATELASTLOGGEDUSER>

    TiXmlElement* defaultGW = new TiXmlElement("DEFAULTGATEWAY");
    defaultGW->LinkEndChild(new TiXmlText("192.168.0.1"));
    //<DEFAULTGATEWAY>192.168.22.9</DEFAULTGATEWAY>

    TiXmlElement* description = new TiXmlElement("DESCRIPTION");
    std::string descriptionString;
    descriptionString.append(fMachine->OSInfo().machine).append("/");
    description->LinkEndChild(new TiXmlText(descriptionString));

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
    memory->LinkEndChild(new TiXmlText(fMachine->OSInfo().memory));

    TiXmlElement* name = new TiXmlElement("NAME");
    name->LinkEndChild(new TiXmlText(fMachine->HostName()));

    TiXmlElement* osComments = new TiXmlElement("OSCOMMENTS");
    osComments->LinkEndChild(new TiXmlText(fMachine->OSInfo().comments));

    TiXmlElement* osName = new TiXmlElement("OSNAME");
    osName->LinkEndChild(new TiXmlText(fMachine->OSInfo().os_description));

    TiXmlElement* osVersion = new TiXmlElement("OSVERSION");
    osVersion->LinkEndChild(new TiXmlText(fMachine->OSInfo().os_release));

    TiXmlElement* processorN = new TiXmlElement("PROCESSORN");
    processorN->LinkEndChild(new TiXmlText(int_to_string(fMachine->CountProcessors())));

    TiXmlElement* processorS = new TiXmlElement("PROCESSORS");
    processorS->LinkEndChild(new TiXmlText(fMachine->ProcessorSpeed(0)));

    TiXmlElement* processorT = new TiXmlElement("PROCESSORT");
    processorT->LinkEndChild(new TiXmlText(fMachine->ProcessorType(0)));

    TiXmlElement* swap = new TiXmlElement("SWAP");
    swap->LinkEndChild(new TiXmlText(fMachine->OSInfo().swap));

    TiXmlElement* userID = new TiXmlElement("USERID");
    // TODO: Fix this
    userID->LinkEndChild(new TiXmlText("root"));
    //<USERID>root</USERID>

    TiXmlElement* uuid = new TiXmlElement("UUID");
    uuid->LinkEndChild(new TiXmlText(fMachine->SystemUUID()));


    TiXmlElement* vmSystem = new TiXmlElement("VMSYSTEM");
    vmSystem->LinkEndChild(new TiXmlText("Physical"));
   // <VMSYSTEM>Xen</VMSYSTEM>

    TiXmlElement* workGroup = new TiXmlElement("WORKGROUP");
    workGroup->LinkEndChild(new TiXmlText(fMachine->OSInfo().domain_name));

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
	IfConfigReader ifCfgReader;

	network_info info;
	while (ifCfgReader.GetNext(info)) {
		if (info.description == "lo")
			continue;

		TiXmlElement* networks = new TiXmlElement("NETWORKS");

		TiXmlElement* description = new TiXmlElement("DESCRIPTION");
		description->LinkEndChild(new TiXmlText(info.description));
		networks->LinkEndChild(description);

		TiXmlElement* driver = new TiXmlElement("DRIVER");
		driver->LinkEndChild(new TiXmlText("pif"));
		networks->LinkEndChild(driver);

		TiXmlElement* ipAddress = new TiXmlElement("IPADDRESS");
		ipAddress->LinkEndChild(new TiXmlText(info.ip_address));
		networks->LinkEndChild(ipAddress);

		TiXmlElement* ipDHCP = new TiXmlElement("IPDHCP");
		ipDHCP->LinkEndChild(new TiXmlText(info.dhcp_ip));
		networks->LinkEndChild(ipDHCP);

		TiXmlElement* gateway = new TiXmlElement("IPGATEWAY");
		gateway->LinkEndChild(new TiXmlText(info.gateway));
		networks->LinkEndChild(gateway);

		TiXmlElement* ipMask = new TiXmlElement("IPMASK");
		ipMask->LinkEndChild(new TiXmlText(info.netmask));
		networks->LinkEndChild(ipMask);

		TiXmlElement* ipSubnet = new TiXmlElement("IPSUBNET");
		ipSubnet->LinkEndChild(new TiXmlText(info.network));
		networks->LinkEndChild(ipSubnet);

		TiXmlElement* mac = new TiXmlElement("MACADDR");
		mac->LinkEndChild(new TiXmlText(info.mac_address));
		networks->LinkEndChild(mac);

		TiXmlElement* pciSlot = new TiXmlElement("PCISLOT");
		pciSlot->LinkEndChild(new TiXmlText(""));
		networks->LinkEndChild(pciSlot);

		TiXmlElement* status = new TiXmlElement("STATUS");
		status->LinkEndChild(new TiXmlText(info.status));
		networks->LinkEndChild(status);

		TiXmlElement* type = new TiXmlElement("TYPE");
		type->LinkEndChild(new TiXmlText(""));
		networks->LinkEndChild(type);

		TiXmlElement* virtualDevice = new TiXmlElement("VIRTUALDEV");
		virtualDevice->LinkEndChild(new TiXmlText(""));
		networks->LinkEndChild(virtualDevice);


		parent->LinkEndChild(networks);
	}
}


void
Inventory::_AddProcessesInfo(TiXmlElement* parent)
{
	RunningProcessesList processList;
	process_info processInfo;
	while (processList.GetNext(processInfo)) {
		TiXmlElement* process = new TiXmlElement("PROCESSES");

		TiXmlElement* cmd = new TiXmlElement("CMD");
		cmd->LinkEndChild(new TiXmlText(processInfo.cmdline.c_str()));

		TiXmlElement* cpuUsage = new TiXmlElement("CPUUSAGE");
		cpuUsage->LinkEndChild(new TiXmlText(""));

		TiXmlElement* mem = new TiXmlElement("MEM");
		mem->LinkEndChild(new TiXmlText(int_to_string(processInfo.memory)));

		TiXmlElement* pid = new TiXmlElement("PID");
		pid->LinkEndChild(new TiXmlText(int_to_string(processInfo.pid)));

		TiXmlElement* started = new TiXmlElement("STARTED");
		started->LinkEndChild(new TiXmlText(""));

		TiXmlElement* tty = new TiXmlElement("TTY");
		tty->LinkEndChild(new TiXmlText(""));

		TiXmlElement* user = new TiXmlElement("USER");
		user->LinkEndChild(new TiXmlText(processInfo.user));

		TiXmlElement* virtualMem = new TiXmlElement("VIRTUALMEMORY");
		virtualMem->LinkEndChild(new TiXmlText(int_to_string(processInfo.virtualmem)));

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

	LoggedUsers usersInfo;
	for (int i = 0; i < usersInfo.Count(); i++) {
		TiXmlElement* login = new TiXmlElement("LOGIN");
		login->LinkEndChild(new TiXmlText(usersInfo.UserAt(i).c_str()));
		users->LinkEndChild(login);
	}
	parent->LinkEndChild(users);
}


bool
Inventory::_WriteProlog(TiXmlDocument& document) const
{
	Configuration* config = Configuration::Get();

	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "UTF-8", "");
	document.LinkEndChild(declaration);

	TiXmlElement* request = new TiXmlElement("REQUEST");
	document.LinkEndChild(request);

	TiXmlElement* deviceID = new TiXmlElement("DEVICEID");
	deviceID->LinkEndChild(new TiXmlText(config->DeviceID().c_str()));
	request->LinkEndChild(deviceID);

	TiXmlElement* query = new TiXmlElement("QUERY");
	query->LinkEndChild(new TiXmlText("PROLOG"));

	request->LinkEndChild(query);

	return true;
}
