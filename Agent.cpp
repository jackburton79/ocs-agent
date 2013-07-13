/*
 * Agent.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"
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
	Inventory* inventory = new Inventory();

	if (inventory->Build()) {
		if (!inventory->Save("test")) {
			std::cerr << "Cannot save output file." << std::endl;
			if (false)
				inventory->Send();
		}
	}

	delete inventory;
}

