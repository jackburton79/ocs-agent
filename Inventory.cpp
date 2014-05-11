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
#include "Screens.h"
#include "Support.h"
#include "VolumeReader.h"


#include "http/HTTP.h"

#include <cstdlib>
#include <iostream>
#include <memory>


#include "tinyxml2/tinyxml2.h"

#define USER_AGENT "OCS-NG_unified_unix_agent_v"

Inventory::Inventory()
	:
	fDocument(NULL),
	fMachine(NULL)
{
    fDocument = new tinyxml2::XMLDocument;
	fMachine = new Machine;
}


Inventory::~Inventory()
{
	delete fDocument;
	delete fMachine;
}


bool
Inventory::Initialize(const char* deviceID)
{
	Clear();
    tinyxml2::XMLDeclaration* declaration = fDocument->NewDeclaration();
    tinyxml2::XMLElement* request = fDocument->NewElement("REQUEST");
	fDocument->LinkEndChild(declaration);
	fDocument->LinkEndChild(request);
    fContent = fDocument->NewElement("CONTENT");
	request->LinkEndChild(fContent);

    tinyxml2::XMLElement* query = fDocument->NewElement("QUERY");
		// TODO: We only do Inventory for now
    query->LinkEndChild(fDocument->NewText("INVENTORY"));
	request->LinkEndChild(query);

    tinyxml2::XMLElement* deviceId = fDocument->NewElement("DEVICEID");
    deviceId->LinkEndChild(fDocument->NewText(deviceID));

	request->LinkEndChild(deviceId);

	return true;
}


void
Inventory::Clear()
{
	fDocument->Clear();
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

    tinyxml2::XMLElement* content = fContent;

	_AddAccountInfo(content);
	_AddBIOSInfo(content);
	_AddCPUsInfo(content);
	_AddDrivesInfo(content);
	_AddHardwareInfo(content);
	_AddNetworksInfo(content);
	_AddProcessesInfo(content);
	_AddSoftwaresInfo(content);
	_AddUsersInfo(content);
	_AddVideosInfo(content);
    _AddMonitorsInfo(content);

	return true;
}


bool
Inventory::Save(const char* name)
{
	std::string fullName;
	fullName.append(name).append(".xml");

    return fDocument->SaveFile(fullName.c_str()) == tinyxml2::XML_NO_ERROR;
}


bool
Inventory::Send(const char* serverUrl)
{
	HTTP httpObject;

	// Send Prolog
    tinyxml2::XMLDocument prolog;
	_WriteProlog(prolog);

	char* prologData = NULL;
	size_t prologLength = 0;
	if (!CompressXml(prolog, prologData, prologLength)) {
		std::cerr << "Inventory::Send(): Error compressing prolog XML" << std::endl;
		return false;
	}

	std::string inventoryUrl(serverUrl);

	HTTPRequestHeader requestHeader;
	requestHeader.SetRequest("POST", inventoryUrl);
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(prologLength);
	requestHeader.SetUserAgent(USER_AGENT);
	if (httpObject.Request(requestHeader, prologData, prologLength) != 0) {
		delete[] prologData;
		std::cerr << "Inventory::Send(): cannot send prolog: ";
		std::cerr << httpObject.ErrorString() << std::endl;
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
			std::cerr << "Inventory::Send(): Failed to read XML response: ";
			std::cerr << httpObject.ErrorString() << std::endl;
			return false;
		}

        tinyxml2::XMLDocument document;
		bool uncompress = UncompressXml(resultData, contentLength, document);
		delete[] resultData;

		if (!uncompress) {
			std::cerr << "Inventory::Send(): Failed to decompress XML response" << std::endl;
			return false;
		}

		// TODO: Do something with the reply
#if 1
		document.Print();
#endif
	}

	char* compressedData = NULL;
	size_t compressedSize;
	if (!CompressXml(*fDocument, compressedData, compressedSize)) {
		std::cerr << "Inventory::Send(): Error compressing inventory XML" << std::endl;
		return false;
	}

	requestHeader = HTTPRequestHeader();
	requestHeader.SetRequest("POST", inventoryUrl);
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(compressedSize);
	requestHeader.SetUserAgent(USER_AGENT);
	if (httpObject.Request(requestHeader, compressedData, compressedSize) != 0) {
		delete[] compressedData;
		std::cerr << "Inventory::Send(): Cannot send inventory: ";
		std::cerr << httpObject.ErrorString() << std::endl;
		return false;
	}

	delete[] compressedData;

	return true;
}


int
Inventory::Checksum() const
{
	return 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 | 512 | 1024
			| 2048 | 8192 | 16384 | 32768 | 65536 | 131072;
}


// Private
void
Inventory::_AddAccountInfo(tinyxml2::XMLElement* parent)
{
    tinyxml2::XMLElement* accountInfo = fDocument->NewElement("ACCOUNTINFO");

	// TODO: ??? We can't store anything
	for (int a = 0; a < 1; a++) {
        tinyxml2::XMLElement* keyName = fDocument->NewElement("KEYNAME");
        keyName->LinkEndChild(fDocument->NewElement("TAG"));

        tinyxml2::XMLElement* keyValue = fDocument->NewElement("KEYVALUE");
        keyValue->LinkEndChild(fDocument->NewElement("NA"));

		accountInfo->LinkEndChild(keyName);
		accountInfo->LinkEndChild(keyValue);
	}

	parent->LinkEndChild(accountInfo);
}


void
Inventory::_AddBIOSInfo(tinyxml2::XMLElement* parent)
{
    tinyxml2::XMLElement* bios = fDocument->NewElement("BIOS");

    tinyxml2::XMLElement* assettag = fDocument->NewElement("ASSETTAG");
    assettag->LinkEndChild(fDocument->NewText(fMachine->AssetTag().c_str()));

    tinyxml2::XMLElement* bdate = fDocument->NewElement("BDATE");
    bdate->LinkEndChild(fDocument->NewText(fMachine->BIOSDate().c_str()));

    tinyxml2::XMLElement* bmanufacturer = fDocument->NewElement("BMANUFACTURER");
    bmanufacturer->LinkEndChild(fDocument->NewText(fMachine->BIOSManufacturer().c_str()));

    tinyxml2::XMLElement* bversion = fDocument->NewElement("BVERSION");
    bversion->LinkEndChild(fDocument->NewText(fMachine->BIOSVersion().c_str()));

    tinyxml2::XMLElement* mmanufacturer = fDocument->NewElement("MMANUFACTURER");
    mmanufacturer->LinkEndChild(fDocument->NewText(fMachine->MachineManufacturer().c_str()));

    tinyxml2::XMLElement* mSerial = fDocument->NewElement("MSN");
    mSerial->LinkEndChild(fDocument->NewText(fMachine->MachineSerialNumber().c_str()));

    tinyxml2::XMLElement* sManufacturer = fDocument->NewElement("SMANUFACTURER");
    sManufacturer->LinkEndChild(fDocument->NewText(fMachine->SystemManufacturer().c_str()));

    tinyxml2::XMLElement* systemModel = fDocument->NewElement("SMODEL");
    systemModel->LinkEndChild(fDocument->NewText(fMachine->SystemModel().c_str()));

    tinyxml2::XMLElement* ssn = fDocument->NewElement("SSN");
    ssn->LinkEndChild(fDocument->NewText(fMachine->SystemSerialNumber().c_str()));

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
Inventory::_AddCPUsInfo(tinyxml2::XMLElement* parent)
{
	// TODO: Check if the fields name and structure are correct.
	for (int i = 0; i < fMachine->CountProcessors(); i++) {
        tinyxml2::XMLElement* cpu = fDocument->NewElement("CPUS");
        tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
        tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
        tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
        tinyxml2::XMLElement* model = fDocument->NewElement("TYPE");

		// TODO: Seems like we should interpretate the vendor_id
		manufacturer->LinkEndChild(
                fDocument->NewText(fMachine->ProcessorManufacturer(i).c_str()));
		serial->LinkEndChild(
                fDocument->NewText(fMachine->ProcessorSerialNumber(i).c_str()));
		speed->LinkEndChild(
                fDocument->NewText(fMachine->ProcessorSpeed(i).c_str()));
		model->LinkEndChild(
                fDocument->NewText(fMachine->ProcessorType(i).c_str()));

		cpu->LinkEndChild(model);
		cpu->LinkEndChild(manufacturer);
		cpu->LinkEndChild(serial);
		cpu->LinkEndChild(speed);

		parent->LinkEndChild(cpu);
	}
}


void
Inventory::_AddDrivesInfo(tinyxml2::XMLElement* parent)
{
	VolumeReader reader;
	volume_info info;
	while (reader.GetNext(info)) {
        tinyxml2::XMLElement* drive = fDocument->NewElement("DRIVES");

        tinyxml2::XMLElement* createDate = fDocument->NewElement("CREATEDATE");
        createDate->LinkEndChild(fDocument->NewText(info.create_date.c_str()));

        tinyxml2::XMLElement* fileSystem = fDocument->NewElement("FILESYSTEM");
        fileSystem->LinkEndChild(fDocument->NewText(info.filesystem.c_str()));

        tinyxml2::XMLElement* freeSpace = fDocument->NewElement("FREE");
        freeSpace->LinkEndChild(fDocument->NewText(int_to_string(info.free).c_str()));

        tinyxml2::XMLElement* label = fDocument->NewElement("LABEL");
        label->LinkEndChild(fDocument->NewText(info.label.c_str()));

        tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
        serial->LinkEndChild(fDocument->NewText(info.serial.c_str()));

        tinyxml2::XMLElement* total = fDocument->NewElement("TOTAL");
        total->LinkEndChild(fDocument->NewText(int_to_string(info.total).c_str()));

        tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
        type->LinkEndChild(fDocument->NewText(info.type.c_str()));

        tinyxml2::XMLElement* volumeName = fDocument->NewElement("VOLUMN");
        volumeName->LinkEndChild(fDocument->NewText(info.name.c_str()));

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
Inventory::_AddHardwareInfo(tinyxml2::XMLElement* parent)
{
    tinyxml2::XMLElement* hardware = fDocument->NewElement("HARDWARE");

    tinyxml2::XMLElement* checksum = fDocument->NewElement("CHECKSUM");

    checksum->LinkEndChild(fDocument->NewText(int_to_string(Checksum()).c_str()));

    tinyxml2::XMLElement* dateLastLoggedUser = fDocument->NewElement("DATELASTLOGGEDUSER");
    dateLastLoggedUser->LinkEndChild(fDocument->NewText("Thu Jul 11 13:24"));
    //<DATELASTLOGGEDUSER>Thu Jul 11 13:24</DATELASTLOGGEDUSER>

    tinyxml2::XMLElement* defaultGW = fDocument->NewElement("DEFAULTGATEWAY");
    defaultGW->LinkEndChild(fDocument->NewText("192.168.0.1"));
    //<DEFAULTGATEWAY>192.168.22.9</DEFAULTGATEWAY>

    tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
    std::string descriptionString;
    descriptionString.append(fMachine->OSInfo().machine).append("/");
    description->LinkEndChild(fDocument->NewText(descriptionString.c_str()));

    tinyxml2::XMLElement* dns = fDocument->NewElement("DNS");
    dns->LinkEndChild(fDocument->NewText("192.168.0.1"));
    //<DNS>192.168.22.23/192.168.22.24</DNS>

    tinyxml2::XMLElement* ipAddress = fDocument->NewElement("IPADDR");
    ipAddress->LinkEndChild(fDocument->NewText("192.168.0.1"));
    //<IPADDR>192.168.22.33</IPADDR>

    tinyxml2::XMLElement* lastLoggedUser = fDocument->NewElement("LASTLOGGEDUSER");
    lastLoggedUser->LinkEndChild(fDocument->NewText("root"));
   // <LASTLOGGEDUSER>root</LASTLOGGEDUSER>

    tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORY");
    memory->LinkEndChild(fDocument->NewText(fMachine->OSInfo().memory.c_str()));

    tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
    name->LinkEndChild(fDocument->NewText(fMachine->HostName().c_str()));

    tinyxml2::XMLElement* osComments = fDocument->NewElement("OSCOMMENTS");
    osComments->LinkEndChild(fDocument->NewText(fMachine->OSInfo().comments.c_str()));

    tinyxml2::XMLElement* osName = fDocument->NewElement("OSNAME");
    osName->LinkEndChild(fDocument->NewText(fMachine->OSInfo().os_description.c_str()));

    tinyxml2::XMLElement* osVersion = fDocument->NewElement("OSVERSION");
    osVersion->LinkEndChild(fDocument->NewText(fMachine->OSInfo().os_release.c_str()));

    tinyxml2::XMLElement* processorN = fDocument->NewElement("PROCESSORN");
    processorN->LinkEndChild(fDocument->NewText(int_to_string(fMachine->CountProcessors()).c_str()));

    tinyxml2::XMLElement* processorS = fDocument->NewElement("PROCESSORS");
    processorS->LinkEndChild(fDocument->NewText(fMachine->ProcessorSpeed(0).c_str()));

    tinyxml2::XMLElement* processorT = fDocument->NewElement("PROCESSORT");
    processorT->LinkEndChild(fDocument->NewText(fMachine->ProcessorType(0).c_str()));

    tinyxml2::XMLElement* swap = fDocument->NewElement("SWAP");
    swap->LinkEndChild(fDocument->NewText(fMachine->OSInfo().swap.c_str()));

    tinyxml2::XMLElement* userID = fDocument->NewElement("USERID");
    // TODO: Fix this
    userID->LinkEndChild(fDocument->NewText("root"));
    //<USERID>root</USERID>

    tinyxml2::XMLElement* uuid = fDocument->NewElement("UUID");
    uuid->LinkEndChild(fDocument->NewText(fMachine->SystemUUID().c_str()));

    tinyxml2::XMLElement* vmSystem = fDocument->NewElement("VMSYSTEM");
    vmSystem->LinkEndChild(fDocument->NewText("Physical"));
   // <VMSYSTEM>Xen</VMSYSTEM>

    tinyxml2::XMLElement* workGroup = fDocument->NewElement("WORKGROUP");
    workGroup->LinkEndChild(fDocument->NewText(fMachine->OSInfo().domain_name.c_str()));

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
Inventory::_AddNetworksInfo(tinyxml2::XMLElement* parent)
{
	IfConfigReader ifCfgReader;

	network_info info;
	while (ifCfgReader.GetNext(info)) {
		if (info.description == "lo")
			continue;

        tinyxml2::XMLElement* networks = fDocument->NewElement("NETWORKS");

        tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
        description->LinkEndChild(fDocument->NewText(info.description.c_str()));
		networks->LinkEndChild(description);

        tinyxml2::XMLElement* driver = fDocument->NewElement("DRIVER");
        driver->LinkEndChild(fDocument->NewText("pif"));
		networks->LinkEndChild(driver);

        tinyxml2::XMLElement* ipAddress = fDocument->NewElement("IPADDRESS");
        ipAddress->LinkEndChild(fDocument->NewText(info.ip_address.c_str()));
		networks->LinkEndChild(ipAddress);

        tinyxml2::XMLElement* ipDHCP = fDocument->NewElement("IPDHCP");
        ipDHCP->LinkEndChild(fDocument->NewText(info.dhcp_ip.c_str()));
		networks->LinkEndChild(ipDHCP);

        tinyxml2::XMLElement* gateway = fDocument->NewElement("IPGATEWAY");
        gateway->LinkEndChild(fDocument->NewText(info.gateway.c_str()));
		networks->LinkEndChild(gateway);

        tinyxml2::XMLElement* ipMask = fDocument->NewElement("IPMASK");
        ipMask->LinkEndChild(fDocument->NewText(info.netmask.c_str()));
		networks->LinkEndChild(ipMask);

        tinyxml2::XMLElement* ipSubnet = fDocument->NewElement("IPSUBNET");
        ipSubnet->LinkEndChild(fDocument->NewText(info.network.c_str()));
		networks->LinkEndChild(ipSubnet);

        tinyxml2::XMLElement* mac = fDocument->NewElement("MACADDR");
        mac->LinkEndChild(fDocument->NewText(info.mac_address.c_str()));
		networks->LinkEndChild(mac);

        tinyxml2::XMLElement* pciSlot = fDocument->NewElement("PCISLOT");
        pciSlot->LinkEndChild(fDocument->NewText(""));
		networks->LinkEndChild(pciSlot);

        tinyxml2::XMLElement* status = fDocument->NewElement("STATUS");
        status->LinkEndChild(fDocument->NewText(info.status.c_str()));
		networks->LinkEndChild(status);

        tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
        type->LinkEndChild(fDocument->NewText(info.type.c_str()));
		networks->LinkEndChild(type);

        tinyxml2::XMLElement* virtualDevice = fDocument->NewElement("VIRTUALDEV");
        virtualDevice->LinkEndChild(fDocument->NewText(""));
		networks->LinkEndChild(virtualDevice);


		parent->LinkEndChild(networks);
	}
}


void
Inventory::_AddProcessesInfo(tinyxml2::XMLElement* parent)
{
	RunningProcessesList processList;
	process_info processInfo;
	while (processList.GetNext(processInfo)) {
        tinyxml2::XMLElement* process = fDocument->NewElement("PROCESSES");

        tinyxml2::XMLElement* cmd = fDocument->NewElement("CMD");
        cmd->LinkEndChild(fDocument->NewText(processInfo.cmdline.c_str()));

        tinyxml2::XMLElement* cpuUsage = fDocument->NewElement("CPUUSAGE");
        cpuUsage->LinkEndChild(fDocument->NewText(""));

        tinyxml2::XMLElement* mem = fDocument->NewElement("MEM");
        mem->LinkEndChild(fDocument->NewText(int_to_string(processInfo.memory).c_str()));

        tinyxml2::XMLElement* pid = fDocument->NewElement("PID");
        pid->LinkEndChild(fDocument->NewText(int_to_string(processInfo.pid).c_str()));

        tinyxml2::XMLElement* started = fDocument->NewElement("STARTED");
        started->LinkEndChild(fDocument->NewText(""));

        tinyxml2::XMLElement* tty = fDocument->NewElement("TTY");
        tty->LinkEndChild(fDocument->NewText(""));

        tinyxml2::XMLElement* user = fDocument->NewElement("USER");
        user->LinkEndChild(fDocument->NewText(processInfo.user.c_str()));

        tinyxml2::XMLElement* virtualMem = fDocument->NewElement("VIRTUALMEMORY");
        virtualMem->LinkEndChild(fDocument->NewText(int_to_string(processInfo.virtualmem).c_str()));

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
Inventory::_AddSoftwaresInfo(tinyxml2::XMLElement* parent)
{

}


void
Inventory::_AddUsersInfo(tinyxml2::XMLElement* parent)
{
    tinyxml2::XMLElement* users = fDocument->NewElement("USERS");

	LoggedUsers usersInfo;
	for (int i = 0; i < usersInfo.Count(); i++) {
        tinyxml2::XMLElement* login = fDocument->NewElement("LOGIN");
        login->LinkEndChild(fDocument->NewText(usersInfo.UserAt(i).c_str()));
		users->LinkEndChild(login);
	}
	parent->LinkEndChild(users);
}


void
Inventory::_AddVideosInfo(tinyxml2::XMLElement* parent)
{
	for (int i = 0; i < fMachine->CountVideos(); i++) {
		video_info info = fMachine->VideoInfoFor(i);

        tinyxml2::XMLElement* video = fDocument->NewElement("VIDEOS");
        tinyxml2::XMLElement* chipset = fDocument->NewElement("CHIPSET");
        chipset->LinkEndChild(fDocument->NewText(info.chipset.c_str()));

        tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORY");
        memory->LinkEndChild(fDocument->NewText(info.memory.c_str()));

        tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
        name->LinkEndChild(fDocument->NewText(info.name.c_str()));

        tinyxml2::XMLElement* resolution = fDocument->NewElement("RESOLUTION");
        resolution->LinkEndChild(fDocument->NewText(info.resolution.c_str()));

		video->LinkEndChild(chipset);
		video->LinkEndChild(memory);
		video->LinkEndChild(name);
		video->LinkEndChild(resolution);

		parent->LinkEndChild(video);
	}
}


void
Inventory::_AddMonitorsInfo(tinyxml2::XMLElement* parent)
{

    Screens screens;

    screen_info info;
    while (screens.GetNext(info)) {
        std::cout << "name: " << info.name << ", manufacturer: " <<  info.manufacturer << std::endl;
        tinyxml2::XMLElement* monitor = fDocument->NewElement("MONITORS");
        tinyxml2::XMLElement* base64 = fDocument->NewElement("BASE64");
        tinyxml2::XMLElement* caption = fDocument->NewElement("CAPTION");
        tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
        tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
        tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
        tinyxml2::XMLElement* uuencode = fDocument->NewElement("UUENCODE");

        manufacturer->LinkEndChild(fDocument->NewText((info.manufacturer.c_str())));
        serial->LinkEndChild(fDocument->NewText((info.serial_number.c_str())));

        monitor->LinkEndChild(base64);
        monitor->LinkEndChild(caption);
        monitor->LinkEndChild(description);
        monitor->LinkEndChild(manufacturer);
        monitor->LinkEndChild(serial);
        monitor->LinkEndChild(uuencode);


        parent->LinkEndChild(monitor);
    }

}


bool
Inventory::_WriteProlog(tinyxml2::XMLDocument& document) const
{
	Configuration* config = Configuration::Get();

    tinyxml2::XMLDeclaration* declaration = fDocument->NewDeclaration();
	document.LinkEndChild(declaration);

    tinyxml2::XMLElement* request = fDocument->NewElement("REQUEST");
	document.LinkEndChild(request);

    tinyxml2::XMLElement* deviceID = fDocument->NewElement("DEVICEID");
    deviceID->LinkEndChild(fDocument->NewText(config->DeviceID().c_str()));
	request->LinkEndChild(deviceID);

    tinyxml2::XMLElement* query = fDocument->NewElement("QUERY");
    query->LinkEndChild(fDocument->NewText("PROLOG"));

	request->LinkEndChild(query);

	return true;
}
