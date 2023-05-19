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
#include "inventoryformat/InventoryFormatOCS.h"

#include <iostream>

Inventory::Inventory()
	:
	fFormat(NULL)
{
	fFormat = new InventoryFormatOCS();
}


Inventory::~Inventory()
{
	delete fFormat;
}


bool
Inventory::Initialize()
{
	fFormat->Clear();

    std::string deviceID = Configuration::Get()->DeviceID();
    if (deviceID.empty()) {
    	deviceID = fFormat->GenerateDeviceID();
    	Configuration::Get()->SetDeviceID(deviceID.c_str());
    }

    Logger::LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s...", deviceID.c_str());

	bool result = fFormat->Initialize();

	Logger::LogFormat(LOG_INFO, "Inventory::Initialize(): Device ID: %s... OK!", deviceID.c_str());

	return result;
}


void
Inventory::Clear()
{
	fFormat->Clear();
}


bool
Inventory::Build(bool noSoftware)
{
	Logger::Log(LOG_INFO, "Building inventory...");

	bool result = fFormat->Build(noSoftware);

	Logger::Log(LOG_INFO, "Building inventory... Done!");
	return result;
}


bool
Inventory::Save(const char* fileName)
{
	if (fileName == NULL)
		return false;

	Logger::LogFormat(LOG_INFO, "Saving %s inventory as %s", Configuration::Get()->DeviceID().c_str(), fileName);

	bool result = fFormat->Save(fileName);
	if (result)
		Logger::Log(LOG_INFO, "Inventory saved correctly!");
	else
		Logger::Log(LOG_INFO, "Failed to save inventory!");

	return result;
}


bool
Inventory::Send(const char* serverUrl)
{
	//Logger& logger = Logger::GetDefault();

	// TODO: Move some stuff from InventoryFormat implementation to here
	return fFormat->Send(serverUrl);
}


void
Inventory::Print()
{
	std::cout << fFormat->ToString() << std::endl;
}
