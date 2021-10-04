/*
 * Inventory.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Inventory.h"

#include "Agent.h"
#include "Configuration.h"
#include "Logger.h"
#include "Machine.h"
#include "NetworkInterface.h"
#include "NetworkRoster.h"
#include "ProcessRoster.h"
#include "Screens.h"
#include "Softwares.h"
#include "StorageRoster.h"
#include "Support.h"
#include "UsersRoster.h"
#include "VolumeRoster.h"
#include "XML.h"

#include "http/HTTP.h"
#include "http/URL.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "tinyxml2/tinyxml2.h"


Inventory::Inventory()
	:
	fDocument(NULL),
	fContent(NULL)
{
	fDocument = new tinyxml2::XMLDocument;
}


Inventory::~Inventory()
{
	delete fDocument;
}


bool
Inventory::Initialize()
{
	Logger& logger = Logger::GetDefault();

	Clear();

    std::string deviceID = Configuration::Get()->DeviceID();
	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s...", deviceID.c_str());
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

 	tinyxml2::XMLElement* deviceIdElement = fDocument->NewElement("DEVICEID");
	deviceIdElement->LinkEndChild(fDocument->NewText(deviceID.c_str()));
	request->LinkEndChild(deviceIdElement);

	logger.LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s... OK!", deviceID.c_str());

	return true;
}


void
Inventory::Clear()
{
	fDocument->Clear();
}


bool
Inventory::Build(bool noSoftware)
{
	Logger& logger = Logger::GetDefault();

	logger.Log(LOG_INFO, "Building inventory...");

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
Inventory::Save(const char* fileName)
{
	if (fileName == NULL)
		return false;

	Logger& logger = Logger::GetDefault();

	logger.LogFormat(LOG_INFO, "Saving %s inventory as %s", Configuration::Get()->DeviceID().c_str(), fileName);

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
	URL inventoryUrl(serverUrl);

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
	requestHeader.SetRequest("POST", inventoryUrl.URLString());
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(prologLength);

	// TODO: Improve.
	if (inventoryUrl.Username() != "") {
		requestHeader.SetAuthentication(HTTP_AUTH_TYPE_BASIC,
				inventoryUrl.Username(), inventoryUrl.Password());
	}

	HTTP httpObject;
	std::string userAgentString;
	for (int c = 0; c < 2; c++) {
		// Try first with the new agent. If it fails, try with the old,
		// safe one.
		userAgentString = (c == 0) ? Agent::AgentString()
			: Agent::LegacyAgentString();
		requestHeader.SetUserAgent(userAgentString);

		logger.Log(LOG_INFO, "Inventory::Send(): Prolog prepared!");
		logger.LogFormat(LOG_DEBUG, "%s", requestHeader.ToString().c_str());
		if (httpObject.Request(requestHeader, prologData, prologLength) != 0) {
			delete[] prologData;
			logger.LogFormat(LOG_INFO, "Inventory::Send(): Failed to send prolog: %s",
						httpObject.ErrorString().c_str());
			return false;
		}

		logger.Log(LOG_INFO, "Inventory::Send(): Prolog Sent!");
		const HTTPResponseHeader& responseHeader = httpObject.LastResponse();
		if (responseHeader.StatusCode() == HTTP_BAD_REQUEST) {
			if (c == 0) {
				logger.LogFormat(LOG_INFO, "Server didn't accept prolog. Try again with standard agent string.");
				continue;
			}
		}

		delete[] prologData;

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

		std::string serverResponse = XML::GetElementText(document, "RESPONSE");
		logger.LogFormat(LOG_INFO, "Inventory::Send(): server replied %s", serverResponse.c_str());
		if (serverResponse == "SEND")
			break;
		logger.LogFormat(LOG_ERR, "Server not ready to accept inventory: %s", serverResponse.c_str());
		return false;
	}

	logger.Log(LOG_INFO, "Inventory::Send(): Compressing XML inventory data... ");
	char* compressedData = NULL;
	size_t compressedSize;
	if (!XML::Compress(*fDocument, compressedData, compressedSize)) {
		logger.Log(LOG_ERR, "Inventory::Send(): error while compressing inventory XML data!");
		return false;
	}

	requestHeader.Clear();
	requestHeader.SetRequest("POST", inventoryUrl.URLString());
	requestHeader.SetValue("Pragma", "no-cache");
	requestHeader.SetValue("Keep-Alive", "300");
	requestHeader.SetValue("Connection", "Keep-Alive, TE");
	requestHeader.SetValue("TE", "deflate, gzip");
	requestHeader.SetContentType("application/x-compress");
	requestHeader.SetContentLength(compressedSize);
	requestHeader.SetUserAgent(userAgentString);
	if (inventoryUrl.Username() != "") {
		requestHeader.SetAuthentication(HTTP_AUTH_TYPE_BASIC,
						inventoryUrl.Username(), inventoryUrl.Password());
	}

	logger.LogFormat(LOG_DEBUG, "%s", requestHeader.ToString().c_str());
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
			| 2048 | 4096 | 8192 | 16384 | 32768 | 65536 | 131072 | 262144;
}


// Private
void
Inventory::_AddAccountInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	tinyxml2::XMLElement* accountInfo = fDocument->NewElement("ACCOUNTINFO");

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
	logger.Log(LOG_DEBUG, "\tAdded Account Info!");
}


void
Inventory::_AddBIOSInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	tinyxml2::XMLElement* bios = fDocument->NewElement("BIOS");

	tinyxml2::XMLElement* assettag = fDocument->NewElement("ASSETTAG");
	assettag->LinkEndChild(fDocument->NewText(gComponents["CHASSIS"].fields["asset_tag"].c_str()));

	tinyxml2::XMLElement* bdate = fDocument->NewElement("BDATE");
	bdate->LinkEndChild(fDocument->NewText(gComponents["BIOS"].fields["release_date"].c_str()));

	tinyxml2::XMLElement* bmanufacturer = fDocument->NewElement("BMANUFACTURER");
	bmanufacturer->LinkEndChild(fDocument->NewText(gComponents["BIOS"].fields["vendor"].c_str()));

	tinyxml2::XMLElement* bversion = fDocument->NewElement("BVERSION");
	bversion->LinkEndChild(fDocument->NewText(gComponents["BIOS"].fields["version"].c_str()));

	tinyxml2::XMLElement* mmanufacturer = fDocument->NewElement("MMANUFACTURER");
	mmanufacturer->LinkEndChild(fDocument->NewText(gComponents["BOARD"].fields["vendor"].c_str()));

	tinyxml2::XMLElement* mSerial = fDocument->NewElement("MSN");
	mSerial->LinkEndChild(fDocument->NewText(gComponents["BOARD"].fields["serial"].c_str()));

	tinyxml2::XMLElement* sManufacturer = fDocument->NewElement("SMANUFACTURER");
	sManufacturer->LinkEndChild(fDocument->NewText(gComponents["SYSTEM"].fields["vendor"].c_str()));

	tinyxml2::XMLElement* systemModel = fDocument->NewElement("SMODEL");
	systemModel->LinkEndChild(fDocument->NewText(gComponents["SYSTEM"].fields["name"].c_str()));

	// TODO: Move into backend(s)
	// Some systems have this empty, or, like our MCP79s,
	// "To Be Filled by O.E.M.", which is pretty much useless,
	// so in that case we use the baseboard serial number
	std::string systemSerial;
	if (gComponents["SYSTEM"].fields["serial"].empty()
		|| gComponents["SYSTEM"].fields["serial"] == "To Be Filled By O.E.M.")
		systemSerial = gComponents["BOARD"].fields["serial"];
	else
		systemSerial = gComponents["SYSTEM"].fields["serial"];

	tinyxml2::XMLElement* ssn = fDocument->NewElement("SSN");
	ssn->LinkEndChild(fDocument->NewText(systemSerial.c_str()));

	tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
	type->LinkEndChild(fDocument->NewText(gComponents["CHASSIS"].fields["type"].c_str()));

	bios->LinkEndChild(assettag);
	bios->LinkEndChild(bdate);
	bios->LinkEndChild(bmanufacturer);
	bios->LinkEndChild(bversion);
	bios->LinkEndChild(mmanufacturer);
	bios->LinkEndChild(mSerial);
	bios->LinkEndChild(sManufacturer);
	bios->LinkEndChild(systemModel);
	bios->LinkEndChild(ssn);
	bios->LinkEndChild(type);

	parent->LinkEndChild(bios);

	logger.Log(LOG_DEBUG, "\tAdded BIOS Info!");
}


void
Inventory::_AddCPUsInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	std::pair<components_map::iterator, components_map::iterator> CPUs = gComponents.equal_range("CPU");
	size_t cpuCount = 0;
	for (components_map::iterator i = CPUs.first; i != CPUs.second; i++) {
		cpuCount++;
		Component& cpuInfo = (*i).second;
		// TODO: This is not completely correct: We could have a 64 bit capable CPU on
		// a 32 bit OS.
		// TODO: Need something like "AddElement("NAME", "VALUE");
		tinyxml2::XMLElement* cpu = fDocument->NewElement("CPUS");
		tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
		tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");
		tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
		tinyxml2::XMLElement* currentSpeed = fDocument->NewElement("CURRENT_SPEED");
		tinyxml2::XMLElement* model = fDocument->NewElement("TYPE");
		tinyxml2::XMLElement* arch = fDocument->NewElement("CPUARCH");
		tinyxml2::XMLElement* dataWidth = fDocument->NewElement("DATA_WIDTH");
		tinyxml2::XMLElement* currentAddressWidth = fDocument->NewElement("CURRENT_ADDRESS_WIDTH");
		tinyxml2::XMLElement* cores = fDocument->NewElement("CORES");
		tinyxml2::XMLElement* cacheSize = fDocument->NewElement("L2CACHESIZE");
		tinyxml2::XMLElement* logicalCpu = fDocument->NewElement("LOGICAL_CPUS");

		std::string dataWidthString = gComponents["CPU"].fields["width"];
		if (dataWidthString.empty()) {
			if (gComponents["OS"].fields["architecture"] == "x86_64")
				dataWidthString = "64";
			else
				dataWidthString = "32";
		}
		manufacturer->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["vendor"].c_str()));
		serial->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["serial"].c_str()));

		speed->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["speed"].c_str()));
		currentSpeed->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["current_speed"].c_str()));
		model->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["type"].c_str()));
		cores->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["cores"].c_str()));
		arch->LinkEndChild(
			fDocument->NewText(gComponents["OS"].fields["architecture"].c_str()));
		dataWidth->LinkEndChild(
			fDocument->NewText(dataWidthString.c_str()));
		// Not a copy/paste error: the fields are the same
		currentAddressWidth->LinkEndChild(
			fDocument->NewText(dataWidthString.c_str()));
		cacheSize->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["cache_size"].c_str()));
		logicalCpu->LinkEndChild(
			fDocument->NewText(cpuInfo.fields["logical_cpus"].c_str()));

		cpu->LinkEndChild(model);
		cpu->LinkEndChild(manufacturer);
		cpu->LinkEndChild(serial);
		cpu->LinkEndChild(speed);
		cpu->LinkEndChild(currentSpeed);
		cpu->LinkEndChild(cores);
		cpu->LinkEndChild(arch);
		cpu->LinkEndChild(dataWidth);
		cpu->LinkEndChild(currentAddressWidth);
		cpu->LinkEndChild(cacheSize);
		cpu->LinkEndChild(logicalCpu);

		parent->LinkEndChild(cpu);
	}
	logger.Log(LOG_DEBUG, "\tAdded CPUs Info!");
}


void
Inventory::_AddStoragesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	StorageRoster storages;
	storage_info info;
	while (storages.GetNext(info)) {
		tinyxml2::XMLElement* storage = fDocument->NewElement("STORAGES");

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

	logger.Log(LOG_DEBUG, "\tAdded Storage Info!");
}


void
Inventory::_AddMemoriesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	int slotNum = 0;
	for (;;) {
		std::ostringstream s;
		s << "MEMORY" << slotNum;
		std::map<std::string, Component>::iterator i = gComponents.find(s.str());
		if (i == gComponents.end())
			break;

		tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORIES");

		tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
		tinyxml2::XMLElement* caption = fDocument->NewElement("CAPTION");
		tinyxml2::XMLElement* capacity = fDocument->NewElement("CAPACITY");
		tinyxml2::XMLElement* purpose = fDocument->NewElement("PURPOSE");
		tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
		tinyxml2::XMLElement* speed = fDocument->NewElement("SPEED");
		tinyxml2::XMLElement* numSlots = fDocument->NewElement("NUMSLOTS");
		tinyxml2::XMLElement* serial = fDocument->NewElement("SERIALNUMBER");

		Component ramSlot = i->second;
		description->LinkEndChild(fDocument->NewText(ramSlot.fields["description"].c_str()));
		memory->LinkEndChild(description);

		caption->LinkEndChild(fDocument->NewText(ramSlot.fields["caption"].c_str()));
		memory->LinkEndChild(caption);

		capacity->LinkEndChild(fDocument->NewText(ramSlot.fields["size"].c_str()));
		memory->LinkEndChild(capacity);

		purpose->LinkEndChild(fDocument->NewText(ramSlot.fields["purpose"].c_str()));
		memory->LinkEndChild(purpose);

		type->LinkEndChild(fDocument->NewText(ramSlot.fields["type"].c_str()));
		memory->LinkEndChild(type);

		speed->LinkEndChild(fDocument->NewText(ramSlot.fields["speed"].c_str()));
		memory->LinkEndChild(speed);

		numSlots->LinkEndChild(fDocument->NewText(int_to_string(slotNum + 1).c_str()));
		memory->LinkEndChild(numSlots);

		serial->LinkEndChild(fDocument->NewText(ramSlot.fields["serial"].c_str()));
		memory->LinkEndChild(serial);

		parent->LinkEndChild(memory);
		slotNum++;
	}
	logger.Log(LOG_DEBUG, "\tAdded Memory Info!");
}


void
Inventory::_AddDrivesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	VolumeRoster reader;
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

	logger.Log(LOG_DEBUG, "\tAdded Drives info!");
}


void
Inventory::_AddHardwareInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	tinyxml2::XMLElement* hardware = fDocument->NewElement("HARDWARE");

	tinyxml2::XMLElement* checksum = fDocument->NewElement("CHECKSUM");
	checksum->LinkEndChild(fDocument->NewText(int_to_string(Checksum()).c_str()));
	hardware->LinkEndChild(checksum);

	// Find first active interface
	NetworkRoster roster;
	NetworkInterface interface;
	unsigned int cookie = 0;
	while (roster.GetNextInterface(&cookie, interface) == 0) {
		if (!interface.IsLoopback() && interface.HasIPAddress()
				&& interface.HasDefaultGateway()) {
			tinyxml2::XMLElement* ipAddress = fDocument->NewElement("IPADDR");
			ipAddress->LinkEndChild(fDocument->NewText(interface.IPAddress().c_str()));
			hardware->LinkEndChild(ipAddress);

			std::string defaultGateway = interface.DefaultGateway();
			tinyxml2::XMLElement* defaultGW = fDocument->NewElement("DEFAULTGATEWAY");
			defaultGW->LinkEndChild(fDocument->NewText(defaultGateway.c_str()));
			hardware->LinkEndChild(defaultGW);
			break;
		}
	}

    Component& osInfo = gComponents["OS"] ;
	tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
	std::string descriptionString;
	descriptionString.append(osInfo.fields["architecture"]).append("/");
	description->LinkEndChild(fDocument->NewText(descriptionString.c_str()));
	hardware->LinkEndChild(description);

	tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORY");
	memory->LinkEndChild(fDocument->NewText(osInfo.fields["memory"].c_str()));
	hardware->LinkEndChild(memory);

	tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
	name->LinkEndChild(fDocument->NewText(osInfo.fields["hostname"].c_str()));
	hardware->LinkEndChild(name);

	tinyxml2::XMLElement* osComments = fDocument->NewElement("OSCOMMENTS");
	osComments->LinkEndChild(fDocument->NewText(osInfo.fields["comments"].c_str()));
	hardware->LinkEndChild(osComments);

	tinyxml2::XMLElement* osName = fDocument->NewElement("OSNAME");
	osName->LinkEndChild(fDocument->NewText(osInfo.fields["description"].c_str()));
	hardware->LinkEndChild(osName);

	tinyxml2::XMLElement* osVersion = fDocument->NewElement("OSVERSION");
	osVersion->LinkEndChild(fDocument->NewText(osInfo.fields["release"].c_str()));
	hardware->LinkEndChild(osVersion);

	std::pair<components_map::iterator, components_map::iterator> CPUs = gComponents.equal_range("CPU");
	size_t cpuCount = 0;
	for (components_map::iterator i = CPUs.first; i != CPUs.second; i++) {
		cpuCount++;
		Component cpuInfo = (*i).second;
		tinyxml2::XMLElement* processorS = fDocument->NewElement("PROCESSORS");
		processorS->LinkEndChild(fDocument->NewText(cpuInfo.fields["speed"].c_str()));
		hardware->LinkEndChild(processorS);

		tinyxml2::XMLElement* processorT = fDocument->NewElement("PROCESSORT");
		processorT->LinkEndChild(fDocument->NewText(cpuInfo.fields["type"].c_str()));
		hardware->LinkEndChild(processorT);
	}
	tinyxml2::XMLElement* processorN = fDocument->NewElement("PROCESSORN");
	processorN->LinkEndChild(fDocument->NewText(int_to_string(cpuCount).c_str()));
	hardware->LinkEndChild(processorN);

	tinyxml2::XMLElement* arch = fDocument->NewElement("ARCH");
	arch->LinkEndChild(fDocument->NewText(osInfo.fields["architecture"].c_str()));
	hardware->LinkEndChild(arch);

	tinyxml2::XMLElement* swap = fDocument->NewElement("SWAP");
	swap->LinkEndChild(fDocument->NewText(osInfo.fields["swap"].c_str()));
	hardware->LinkEndChild(swap);

	tinyxml2::XMLElement* userID = fDocument->NewElement("USERID");
	// TODO: Fix this. Use the process user ?
	userID->LinkEndChild(fDocument->NewText("root"));
	hardware->LinkEndChild(userID);

	tinyxml2::XMLElement* uuid = fDocument->NewElement("UUID");
	uuid->LinkEndChild(fDocument->NewText(gComponents["SYSTEM"].fields["uuid"].c_str()));
	hardware->LinkEndChild(uuid);

	tinyxml2::XMLElement* vmSystem = fDocument->NewElement("VMSYSTEM");
	vmSystem->LinkEndChild(fDocument->NewText("Physical"));
	hardware->LinkEndChild(vmSystem);

	tinyxml2::XMLElement* workGroup = fDocument->NewElement("WORKGROUP");
	workGroup->LinkEndChild(fDocument->NewText(osInfo.fields["domainname"].c_str()));
	hardware->LinkEndChild(workGroup);

	UsersRoster users;
	user_entry user;
	if (users.GetNext(user)) {
		tinyxml2::XMLElement* dateLastLoggedUser = fDocument->NewElement("DATELASTLOGGEDUSER");
		dateLastLoggedUser->LinkEndChild(fDocument->NewText(user.logintimestring.c_str()));
		hardware->LinkEndChild(dateLastLoggedUser);
		tinyxml2::XMLElement* lastLoggedUser = fDocument->NewElement("LASTLOGGEDUSER");
		lastLoggedUser->LinkEndChild(fDocument->NewText(user.login.c_str()));
		hardware->LinkEndChild(lastLoggedUser);
	}

	parent->LinkEndChild(hardware);

	logger.Log(LOG_DEBUG, "\tAdded Hardware info!");
}


void
Inventory::_AddNetworksInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	NetworkRoster roster;
	NetworkInterface interface;
	unsigned int cookie = 0;
	while (roster.GetNextInterface(&cookie, interface) == 0) {
		if (interface.IsLoopback())
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

		ipSubnet->LinkEndChild(fDocument->NewText(interface.Network().c_str()));
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

	logger.Log(LOG_DEBUG, "\tAdded Networks info!");
}


void
Inventory::_AddProcessesInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

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

	logger.Log(LOG_DEBUG, "\tAdded Processes list!");
}


void
Inventory::_AddSoftwaresInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	Softwares softwares;
	software_info info;
	while (softwares.GetNext(info)) {
		tinyxml2::XMLElement* software = fDocument->NewElement("SOFTWARES");

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
	logger.Log(LOG_DEBUG, "\tAdded Software list!");
}


void
Inventory::_AddUsersInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	tinyxml2::XMLElement* users = fDocument->NewElement("USERS");

	UsersRoster usersInfo;
	user_entry userEntry;
	while (usersInfo.GetNext(userEntry)) {
		tinyxml2::XMLElement* login = fDocument->NewElement("LOGIN");
		login->LinkEndChild(fDocument->NewText(userEntry.login.c_str()));
		users->LinkEndChild(login);
	}
	parent->LinkEndChild(users);

	logger.Log(LOG_DEBUG, "\tAdded User info!");
}


void
Inventory::_AddVideosInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	// TODO: Multiple video cards
	for (int i = 0; i < 1; i++) {
		Component& info = gComponents["GRAPHICS"];

		tinyxml2::XMLElement* video = fDocument->NewElement("VIDEOS");

		// OCSInventory uses the name as chipset, and the chipset as name
		tinyxml2::XMLElement* chipset = fDocument->NewElement("CHIPSET");
		chipset->LinkEndChild(fDocument->NewText(info.fields["name"].c_str()));

		tinyxml2::XMLElement* memory = fDocument->NewElement("MEMORY");
		memory->LinkEndChild(fDocument->NewText(info.fields["memory_size"].c_str()));

		tinyxml2::XMLElement* name = fDocument->NewElement("NAME");
		name->LinkEndChild(fDocument->NewText(info.fields["type"].c_str()));

		tinyxml2::XMLElement* resolution = fDocument->NewElement("RESOLUTION");
		resolution->LinkEndChild(fDocument->NewText(info.fields["resolution"].c_str()));

		video->LinkEndChild(chipset);
		video->LinkEndChild(memory);
		video->LinkEndChild(name);
		video->LinkEndChild(resolution);

		parent->LinkEndChild(video);
	}
	logger.Log(LOG_DEBUG, "\tAdded Video info!");
}


void
Inventory::_AddMonitorsInfo(tinyxml2::XMLElement* parent)
{
	Logger& logger = Logger::GetDefault();

	Screens screens;
	screen_info info;
	while (screens.GetNext(info)) {
		tinyxml2::XMLElement* monitor = fDocument->NewElement("MONITORS");
		tinyxml2::XMLElement* caption = fDocument->NewElement("CAPTION");
		tinyxml2::XMLElement* description = fDocument->NewElement("DESCRIPTION");
		tinyxml2::XMLElement* type = fDocument->NewElement("TYPE");
		tinyxml2::XMLElement* manufacturer = fDocument->NewElement("MANUFACTURER");
		tinyxml2::XMLElement* serial = fDocument->NewElement("SERIAL");

		caption->LinkEndChild(fDocument->NewText(info.model.c_str()));
		description->LinkEndChild(fDocument->NewText(info.description.c_str()));
		type->LinkEndChild(fDocument->NewText(info.type.c_str()));
		manufacturer->LinkEndChild(fDocument->NewText(info.manufacturer.c_str()));
		serial->LinkEndChild(fDocument->NewText(info.serial_number.c_str()));

		monitor->LinkEndChild(caption);
		monitor->LinkEndChild(description);
		monitor->LinkEndChild(type);
		monitor->LinkEndChild(manufacturer);
		monitor->LinkEndChild(serial);

		parent->LinkEndChild(monitor);
	}

	logger.Log(LOG_DEBUG, "\tAdded Display info!");
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
