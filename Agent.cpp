/*
 * Agent.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"
#include "Configuration.h"
#include "Inventory.h"

#include <iostream>
#include <unistd.h>

Agent::Agent()
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

	if (inventory.Build(config->DeviceID().c_str())) {
		if (!config->LocalInventory()) {
			if (inventory.Send(config->ServerURL().c_str()))
				return;

			std::cerr << "Cannot send inventory. Will try to save it locally." << std::endl;
		}
		if (!inventory.Save(config->DeviceID().c_str()))
			std::cerr << "Cannot save inventory file." << std::endl;

	}
}

