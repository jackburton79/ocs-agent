/*
 * Agent.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Agent.h"
#include "Configuration.h"
#include "Inventory.h"

#include <iostream>
#include <unistd.h>

Agent::Agent()
	:
	fNoSoftwareInventory(false)
{
	if (geteuid() != 0) {
		throw std::string("This program needs to be run as root");
	}
}


Agent::~Agent()
{
}


void
Agent::Run()
{
	Configuration* config = Configuration::Get();
	Inventory inventory;

	if (!inventory.Initialize(config->DeviceID().c_str()))
		throw "Cannot initialize Inventory";

	if (inventory.Build(config->DeviceID().c_str(), NoSoftwareInventory())) {
		if (!config->LocalInventory()) {
			if (inventory.Send(config->ServerURL().c_str()))
				return;

			std::cerr << "Cannot send inventory. Will try to save it locally." << std::endl;
		}
		std::string fullFileName = config->OutputFileName();
		if (fullFileName.empty()) {
			std::cerr << "Cannot save inventory: no path or file name specified." << std::endl;
		} else {
			if (fullFileName[fullFileName.size() - 1] == '/')
				fullFileName.append(config->DeviceID().c_str()).append(".xml");
			if (!inventory.Save(config->DeviceID().c_str(), fullFileName.c_str()))
				std::cerr << "Cannot save inventory file." << std::endl;
		}
	}
}


bool
Agent::NoSoftwareInventory() const
{
	return fNoSoftwareInventory;
}


void
Agent::SetNoSoftwareInventory(bool value)
{
	fNoSoftwareInventory = value;
}
