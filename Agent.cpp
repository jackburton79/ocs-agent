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
		std::cerr << "This app needs to be run as root" << std::endl;
		throw "error";
	}
}


Agent::~Agent()
{
}


void
Agent::Run()
{
	Configuration config;

	Inventory inventory;

	if (inventory.Build()) {
		if (!inventory.Save("test")) {
			std::cerr << "Cannot save output file." << std::endl;
			if (!config.LocalInventory())
				inventory.Send(config.ServerURL().c_str());
		}
	}
}

