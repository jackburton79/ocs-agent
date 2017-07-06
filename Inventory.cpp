/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Configuration.h"
#include "Inventory.h"
#include "Logger.h"
#include "LoggedUsers.h"
#include "Machine.h"
#include "RunningProcessesList.h"
#include "Screens.h"
#include "Softwares.h"
#include "Storages.h"
#include "Support.h"
#include "VolumeReader.h"
#include "XML.h"

#include "http/HTTP.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "NetworkInterface.h"
#include "NetworkRoster.h"
#include "tinyxml2/tinyxml2.h"

#define USER_AGENT "OCS-NG_unified_unix_agent_v"

Inventory::Inventory()
	:
	fDocument(NULL),
	fContent(NULL),
	fMachine(NULL)
{
	fDocument = new tinyxml2::XMLDocument;
	fMachine = Machine::Get();
}


Inventory::~Inventory()
{
	delete fDocument;
}


bool
Inventory::Initialize(const char* deviceID)
{
	Logger& logger = Logger::GetDefault();

	Clear();

	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s...", deviceID);
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

	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s... OK!", deviceID);

	return true;
}


void
Inventory::Clear()
{
	fDocument->Clear();
}


bool
Inventory::Build(const char* deviceID, bool noSoftware)
{
	Logger& logger = Logger::GetDefault();

	logger.Log(LOG_INFO, "Building inventory...");

	// TODO: Finish this, cleanup.

	tinyxml2::XMLElement* content = fContent;

	try {
		_AddAccountInfo(content);
		_AddBIOSInfo(content);
		_AddCPUsInfo(content);
		_AddStoragesInfo(content);
		_AddDrivesInfo(content);
		_AddMemoriesInfo(content);
		_AddHardwareInfo(content);
		_AddNetworksInfo(content);
		_AddProcessesInfo(content);
		if (!noSoftware)
			_AddSoftwaresInfo(content);
		_AddUsersInfo(content);
		_AddVideosInfo(content);
		_AddMonitorsInfo(content);
	} catch (...) {
		// Something failed.
	}
	logger.Log(LOG_INFO, "Building inventory... Done!");
	return true;
}


bool
Inventory::Save(const char* name, const char* fileName)
{
	if (name == NULL || fileName == NULL)
		return false;
		
	Logger& logger = Logger::GetDefault();
	
	logger.LogFormat(LOG_INFO, "Saving %s inventory as %s", name, fileName);

	bool result = fDocument->SaveFile(fileName) == tinyxml2::XML_SUCCESS;
	if (result)
		logger.Log(LOG_INFO, "Inventory saved correctly!");
	else
		logger.Log(LOG_INFO, "Failed to save inventory!");

	return result;
}


bool
Inventory::Send(const char* serverUrl)
{
	std::string inventoryUrl(serverUrl);

	Logger& logger = Logger::GetDefault();

	// Prepare prolog
	logger.LogFormat(LOG_INFO, "Inventory::Send(): server URL: %s", serverUrl);
	tinyxml2::XMLDocument prolog;
	_WriteProlog(prolog);
	char* prologData = NULL;
	size_t prologLength = 0;
	if (!XML::Compress(prolog, prologData, prologLength)) {
		logger.Log(LOG_ERR, "Error while compressing XML prolog!");
		return false;
	}

	HTTPRequestHeader requestHeader;
	requestHeader.SetRequest("POST", inventoryUrl);
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(prologLength);
	requestHeader.SetUserAgent(USER_AGENT);
	HTTP httpObject;
	
	logger.Log(LOG_INFO, "Inventory::Send(): Prolog prepared!");
	if (httpObject.Request(requestHeader, prologData, prologLength) != 0) {
		delete[] prologData;
		logger.LogFormat(LOG_INFO, "Inventory::Send(): Failed to send prolog: %s",
					httpObject.ErrorString().c_str());
		return false;
	}

	delete[] prologData;

	logger.Log(LOG_INFO, "Inventory::Send(): Prolog Sent!");
	const HTTPResponseHeader& responseHeader = httpObject.LastResponse();
	if (responseHeader.StatusCode() != HTTP_OK
			|| !responseHeader.HasContentLength()) {
		logger.LogFormat(LOG_ERR, "Server replied %s", responseHeader.StatusString().c_str());
		logger.LogFormat(LOG_ERR, "%s", responseHeader.ToString().c_str());
		return false;
	}

	size_t contentLength = ::strtol(responseHeader.Value(HTTPContentLength).c_str(), NULL, 10);
	char* resultData = new char[contentLength];
	if (httpObject.Read(resultData, contentLength) < (int)contentLength) {
		delete[] resultData;
		logger.LogFormat(LOG_ERR, "Inventory::Send(): failed to read XML response: %s",
				httpObject.ErrorString().c_str());

		return false;
	}

	logger.Log(LOG_INFO, "Inventory::Send(): Decompressing XML... ");
	tinyxml2::XMLDocument document;
	bool uncompress = XML::Uncompress(resultData, contentLength, document);
	delete[] resultData;
	if (!uncompress) {
		logger.Log(LOG_ERR, "failed to decompress XML");
		return false;
	}
		
	std::string serverResponse = XML::GetTextElementValue(document, "RESPONSE");
	if (serverResponse != "SEND") {
		logger.LogFormat(LOG_ERR, "Server not ready to accept inventory: %s", serverResponse.c_str());
		return false;
	}
	
	logger.LogFormat(LOG_INFO, "Inventory::Send(): server replied %s", serverResponse.c_str());

	std::cerr << "Inventory::Send(): Compressing XML inventory data... ";
	char* compressedData = NULL;
	size_t compressedSize;
	if (!XML::Compress(*fDocument, compressedData, compressedSize)) {
		logger.Log(LOG_ERR, "Inventory::Send(): error while compressing inventory XML data!");
		return false;
	}

	requestHeader.Clear();
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
		logger.LogFormat(LOG_ERR, "Inventory::Send(): error while sending inventory: %s",
				httpObject.ErrorString().c_str());
		return false;
	}

	logger.Log(LOG_INFO, "Inventory::Send(): Inventory sent correctly!");

	delete[] compressedData;

	return true;
}


void
Inventory::Print()
{
	std::cout << XML::ToString(*fDocument) << std::endl;
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
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Account Info...");
	
	tinyxml2::XMLElement* accountInfo = fDocument->NewElement("ACCOUNTINFO");

	// TODO: ??? We can't store anything
	tinyxml2::XMLElement* keyName = fDocument->NewElement("KEYNAME");
	keyName->LinkEndChild(fDocument->NewText("TAG"));

	std::string tag = Configuration::Get()->KeyValue("TAG");
	if (tag == "")
		tag = "NA";
		
	tinyxml2::XMLElement* keyValue = fDocument->NewElement("KEYVALUE");
	keyValue->LinkEndChild(fDocument->NewText(tag.c_str()));

	accountInfo->LinkEndChild(keyName);
	accountInfo->LinkEndChild(keyValue);

	parent->LinkEndChild(accountInfo);
	logger.Log(LOG_INFO, "\tDone adding Account Info!");
}


void
Inventory::_AddBIOSInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding BIOS Info...");
	
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
	
	logger.Log(LOG_INFO, "\tDone adding BIOS Info!");
}


void
Inventory::_AddCPUsInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding CPUs Info...");
	
	// TODO: Check if the fields name and structure are correct.
	for (int i = 0; i < fMachine->CountProcessors(); i++) {
		tinyxml2::XMLElement* cpu = fDocument->NewElement("CPUS");
		tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
		tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
		tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
		tinyxml2::XMLElement* model = fDocument->NewElement("TYPE");
		tinyxml2::XMLElement* cores = fDocument->NewElement("CORES");

		// TODO: Seems like we should interpretate the vendor_id
		manufacturer->LinkEndChild(
			fDocument->NewText(fMachine->ProcessorManufacturer(i).c_str()));
		serial->LinkEndChild(
			fDocument->NewText(fMachine->ProcessorSerialNumber(i).c_str()));
		speed->LinkEndChild(
			fDocument->NewText(fMachine->ProcessorSpeed(i).c_str()));
		model->LinkEndChild(
			fDocument->NewText(fMachine->ProcessorType(i).c_str()));
		cores->LinkEndChild(
			fDocument->NewText(fMachine->ProcessorCores(i).c_str()));

		cpu->LinkEndChild(model);
		cpu->LinkEndChild(manufacturer);
		cpu->LinkEndChild(serial);
		cpu->LinkEndChild(speed);
		cpu->LinkEndChild(cores);

		parent->LinkEndChild(cpu);
	}
	logger.Log(LOG_INFO, "\tDone adding CPUs Info!");
}


void
Inventory::_AddStoragesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Storages Info...");
	
	Storages storages;
	for (int i = 0; i < storages.Count(); i++) {
		tinyxml2::XMLElement* storage = fDocument->NewElement("STORAGES");
		storage_info info = storages.StorageAt(i);
#if 0		
		std::cout << std::endl;
		std::cout << "Storage number " << i << std::endl;
		std::cout << "manufacturer: " << info.manufacturer << std::endl;
		std::cout << "model: " << info.model << std::endl;
#endif
		tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
		manufacturer->LinkEndChild(fDocument->NewText(info.manufacturer.c_str()));

		tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
		name->LinkEndChild(fDocument->NewText(info.name.c_str()));
		
		tinyxml2::XMLElement* model = fDocument->NewElement("MODEL");
		model->LinkEndChild(fDocument->NewText(info.model.c_str()));

		tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
		description->LinkEndChild(fDocument->NewText(info.description.c_str()));

		tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
		type->LinkEndChild(fDocument->NewText(info.type.c_str()));
		
		tinyxml2::XMLElement* diskSize = fDocument->NewElement("DISKSIZE");
		diskSize->LinkEndChild(fDocument->NewText(info.size.c_str()));

		tinyxml2::XMLElement* serialNumber = fDocument->NewElement("SERIALNUMBER");
		serialNumber->LinkEndChild(fDocument->NewText(info.serial_number.c_str()));
		
		tinyxml2::XMLElement* firmware = fDocument->NewElement("FIRMWARE");
		firmware->LinkEndChild(fDocument->NewText(info.firmware.c_str()));
		
		storage->LinkEndChild(manufacturer);
		storage->LinkEndChild(name);
		storage->LinkEndChild(model);
		storage->LinkEndChild(description);
		storage->LinkEndChild(type);
		storage->LinkEndChild(diskSize);
		storage->LinkEndChild(serialNumber);
		storage->LinkEndChild(firmware);

		parent->LinkEndChild(storage);
	}

	logger.Log(LOG_INFO, "\tDone adding Storages Info!");
}


void
Inventory::_AddMemoriesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Memory Info...");

	for (int i = 0; i < fMachine->CountMemories(); i++) {
		tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORIES");

		tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
		tinyxml2::XMLElement* capacity = fDocument->NewElement("CAPACITY");
		tinyxml2::XMLElement* purpose = fDocument->NewElement("PURPOSE");
		tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
		tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
		tinyxml2::XMLElement* numSlots = fDocument->NewElement("NUMSLOTS");
		tinyxml2::XMLElement* serial = fDocument->NewElement("SERIALNUMBER");

		description->LinkEndChild(fDocument->NewText(fMachine->MemoryDescription(i).c_str()));
		memory->LinkEndChild(description);

		capacity->LinkEndChild(fDocument->NewText(fMachine->MemoryCapacity(i).c_str()));
		memory->LinkEndChild(capacity);

		purpose->LinkEndChild(fDocument->NewText(fMachine->MemoryPurpose(i).c_str()));
		memory->LinkEndChild(purpose);

		type->LinkEndChild(fDocument->NewText(fMachine->MemoryType(i).c_str()));
		memory->LinkEndChild(type);

		speed->LinkEndChild(fDocument->NewText(fMachine->MemorySpeed(i).c_str()));
		memory->LinkEndChild(speed);

		numSlots->LinkEndChild(fDocument->NewText(fMachine->MemoryNumSlot(i).c_str()));
		memory->LinkEndChild(numSlots);

		serial->LinkEndChild(fDocument->NewText(fMachine->MemorySerialNumber(i).c_str()));
		memory->LinkEndChild(serial);

		parent->LinkEndChild(memory);
	}
	logger.Log(LOG_INFO, "\tDone adding Memory Info!");
}


void
Inventory::_AddDrivesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Drives info...");
	
	try {
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
	} catch (...) {
	}
	logger.Log(LOG_INFO, "\tDone adding Drives info!");
}


void
Inventory::_AddHardwareInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Hardware info...");
	
	tinyxml2::XMLElement* hardware = fDocument->NewElement("HARDWARE");

	tinyxml2::XMLElement* checksum = fDocument->NewElement("CHECKSUM");

	checksum->LinkEndChild(fDocument->NewText(int_to_string(Checksum()).c_str()));

	// Find first active interface
	NetworkRoster roster;
	NetworkInterface interface;
	unsigned int cookie = 0;
	while (roster.GetNextInterface(&cookie, interface) == 0) {
		if (interface.Name() != "lo" && interface.IPAddress() != ""
				&& interface.IPAddress() != "0.0.0.0")
			break;
	}

	std::string defaultGateway = interface.DefaultGateway();
	tinyxml2::XMLElement* defaultGW = fDocument->NewElement("DEFAULTGATEWAY");
	defaultGW->LinkEndChild(fDocument->NewText(defaultGateway.c_str()));

	tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
	std::string descriptionString;
	descriptionString.append(fMachine->OSInfo().machine).append("/");
	description->LinkEndChild(fDocument->NewText(descriptionString.c_str()));

	tinyxml2::XMLElement* ipAddress = fDocument->NewElement("IPADDR");
	ipAddress->LinkEndChild(fDocument->NewText(interface.IPAddress().c_str()));

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
	hardware->LinkEndChild(defaultGW);
	hardware->LinkEndChild(description);
	hardware->LinkEndChild(ipAddress);

	try {
		LoggedUsers users;
		if (users.Count() > 0) {
			user_entry user = users.UserEntryAt(users.Count() - 1);
			tinyxml2::XMLElement* dateLastLoggedUser = fDocument->NewElement("DATELASTLOGGEDUSER");
			dateLastLoggedUser->LinkEndChild(fDocument->NewText(user.logintimestring.c_str()));
			hardware->LinkEndChild(dateLastLoggedUser);
			tinyxml2::XMLElement* lastLoggedUser = fDocument->NewElement("LASTLOGGEDUSER");
			lastLoggedUser->LinkEndChild(fDocument->NewText(user.login.c_str()));
			hardware->LinkEndChild(lastLoggedUser);
		}
	} catch (...) {
		// Not a big issue, after all.
	}

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
	
	logger.Log(LOG_INFO, "\tDone adding Hardware info!");
}


void
Inventory::_AddNetworksInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Networks info...");
	
	try {
		NetworkRoster roster;
		NetworkInterface interface;
		unsigned int cookie = 0;
		while (roster.GetNextInterface(&cookie, interface) == 0) {
			if (interface.Name() == "lo")
				continue;

			tinyxml2::XMLElement* networks = fDocument->NewElement("NETWORKS");

			tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
			description->LinkEndChild(fDocument->NewText(interface.Name().c_str()));
			networks->LinkEndChild(description);

			tinyxml2::XMLElement* driver = fDocument->NewElement("DRIVER");
			driver->LinkEndChild(fDocument->NewText("pif"));
			networks->LinkEndChild(driver);

			tinyxml2::XMLElement* ipAddress = fDocument->NewElement("IPADDRESS");
			ipAddress->LinkEndChild(fDocument->NewText(interface.IPAddress().c_str()));
			networks->LinkEndChild(ipAddress);

			tinyxml2::XMLElement* ipDHCP = fDocument->NewElement("IPDHCP");
			ipDHCP->LinkEndChild(fDocument->NewText(""));
			networks->LinkEndChild(ipDHCP);

			tinyxml2::XMLElement* gateway = fDocument->NewElement("IPGATEWAY");
			std::string gatewayString = interface.DefaultGateway();
			gateway->LinkEndChild(fDocument->NewText(gatewayString.c_str()));
			networks->LinkEndChild(gateway);

			tinyxml2::XMLElement* ipMask = fDocument->NewElement("IPMASK");
			ipMask->LinkEndChild(fDocument->NewText(interface.NetMask().c_str()));
			networks->LinkEndChild(ipMask);

			tinyxml2::XMLElement* ipSubnet = fDocument->NewElement("IPSUBNET");
			ipSubnet->LinkEndChild(fDocument->NewText(interface.BroadcastAddress().c_str()));
			networks->LinkEndChild(ipSubnet);

			tinyxml2::XMLElement* mac = fDocument->NewElement("MACADDR");
			mac->LinkEndChild(fDocument->NewText(interface.HardwareAddress().c_str()));
			networks->LinkEndChild(mac);

			tinyxml2::XMLElement* pciSlot = fDocument->NewElement("PCISLOT");
			pciSlot->LinkEndChild(fDocument->NewText(""));
			networks->LinkEndChild(pciSlot);

			tinyxml2::XMLElement* status = fDocument->NewElement("STATUS");
			status->LinkEndChild(fDocument->NewText(interface.Status().c_str()));
			networks->LinkEndChild(status);

			tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
			type->LinkEndChild(fDocument->NewText(interface.Type().c_str()));
			networks->LinkEndChild(type);

			tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
			speed->LinkEndChild(fDocument->NewText(interface.Speed().c_str()));
			networks->LinkEndChild(speed);

			tinyxml2::XMLElement* virtualDevice = fDocument->NewElement("VIRTUALDEV");
			virtualDevice->LinkEndChild(fDocument->NewText(""));
			networks->LinkEndChild(virtualDevice);

			parent->LinkEndChild(networks);
		}
	} catch (...) {
	}
	logger.Log(LOG_INFO, "\tDone adding Networks info!");
}


void
Inventory::_AddProcessesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Processes list...");

	try {
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
	} catch (...) {
	}
		
	logger.Log(LOG_INFO, "\tDone adding Processes list!");
}


void
Inventory::_AddSoftwaresInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Software list...");

	Softwares softwares;
	for (int i = 0; i < softwares.Count(); i++) {
		tinyxml2::XMLElement* software = fDocument->NewElement("SOFTWARES");
		software_info info = softwares.SoftwareAt(i);

		tinyxml2::XMLElement* comments = fDocument->NewElement("COMMENTS");
		comments->LinkEndChild(fDocument->NewText(info.comments.c_str()));

		tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
		name->LinkEndChild(fDocument->NewText(info.name.c_str()));

		tinyxml2::XMLElement* size = fDocument->NewElement("FILESIZE");
		size->LinkEndChild(fDocument->NewText(info.size.c_str()));

		tinyxml2::XMLElement* from = fDocument->NewElement("FROM");
		from->LinkEndChild(fDocument->NewText(info.from.c_str()));

		tinyxml2::XMLElement* installdate = fDocument->NewElement("INSTALLDATE");
		installdate->LinkEndChild(fDocument->NewText(info.installdate.c_str()));

		tinyxml2::XMLElement* version = fDocument->NewElement("VERSION");
		version->LinkEndChild(fDocument->NewText(info.version.c_str()));

		software->LinkEndChild(comments);
		software->LinkEndChild(name);
		software->LinkEndChild(size);
		software->LinkEndChild(from);
		software->LinkEndChild(installdate);
		software->LinkEndChild(version);

		parent->LinkEndChild(software);
	}
	logger.Log(LOG_INFO, "\tDone adding Software list!");
}


void
Inventory::_AddUsersInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Users info...");
	
	try {
		tinyxml2::XMLElement* users = fDocument->NewElement("USERS");

		LoggedUsers usersInfo;
		for (int i = 0; i < usersInfo.Count(); i++) {
			tinyxml2::XMLElement* login = fDocument->NewElement("LOGIN");
			login->LinkEndChild(fDocument->NewText(usersInfo.LoginNameAt(i).c_str()));
			users->LinkEndChild(login);
		}
		parent->LinkEndChild(users);
	} catch (...) {
	}
	logger.Log(LOG_INFO, "\tDone adding User info!");
}


void
Inventory::_AddVideosInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Video info...");

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
	logger.Log(LOG_INFO, "\tDone adding Video info!");
}


void
Inventory::_AddMonitorsInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();
	logger.Log(LOG_INFO, "\tAdding Display info...");
	
	try {
		Screens screens;
		screen_info info;
		while (screens.GetNext(info)) {
			tinyxml2::XMLElement* monitor = fDocument->NewElement("MONITORS");
			tinyxml2::XMLElement* caption = fDocument->NewElement("CAPTION");
			tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
			tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
			tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
	
			caption->LinkEndChild(fDocument->NewText(info.model.c_str()));
			description->LinkEndChild(fDocument->NewText(info.description.c_str()));
			manufacturer->LinkEndChild(fDocument->NewText(info.manufacturer.c_str()));
			serial->LinkEndChild(fDocument->NewText(info.serial_number.c_str()));

			monitor->LinkEndChild(caption);
			monitor->LinkEndChild(description);
			monitor->LinkEndChild(manufacturer);
			monitor->LinkEndChild(serial);
	
			parent->LinkEndChild(monitor);
		}
	} catch (...) {
	}
	
	logger.Log(LOG_INFO, "\tDone adding Display info!");
}


bool
Inventory::_WriteProlog(tinyxml2::XMLDocument& document) const
{
	Configuration* config = Configuration::Get();

	tinyxml2::XMLDeclaration* declaration = document.NewDeclaration();
	document.LinkEndChild(declaration);
	tinyxml2::XMLElement* request = document.NewElement("REQUEST");
	document.LinkEndChild(request);
	tinyxml2::XMLElement* deviceID = document.NewElement("DEVICEID");
	deviceID->LinkEndChild(document.NewText(config->DeviceID().c_str()));
	request->LinkEndChild(deviceID);
	tinyxml2::XMLElement* query = document.NewElement("QUERY");
	query->LinkEndChild(document.NewText("PROLOG"));
	request->LinkEndChild(query);

	return true;
}
