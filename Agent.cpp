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
		std::cerr << "This program needs to be run as root" << std::endl;
		throw "error";
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

	if (inventory.Build(config->DeviceID().c_str())) {
		if (config->LocalInventory()) {
			if (!inventory.Save("test"))
				std::cerr << "Cannot save output file." << std::endl;
		} else {
			if (!inventory.Send(config->ServerURL().c_str()))
				std::cerr << "Cannot send inventory." << std::endl;

		}
	}
}

