/*
 * Agent.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Agent.h"
#include "Configuration.h"
#include "Inventory.h"

#include <cstdlib>
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
	// TODO: Either use Logger or just throw.
	// Don't log directly to stdout/stderr.
	Configuration* config = Configuration::Get();
	std::string deviceID = config->DeviceID();
	Inventory inventory;
	if (!inventory.Initialize(deviceID.c_str()))
		throw "Cannot initialize Inventory";

	bool noSoftware = (config->KeyValue("nosoftware") == "true");
	if (!inventory.Build(deviceID.c_str(), noSoftware))
		return;

	if (config->KeyValue("stdout") == "true")
		inventory.Print();
	else if (config->LocalInventory()) {
		std::string fullFileName = config->OutputFileName();
		if (!fullFileName.empty()) {
			if (fullFileName[fullFileName.length() - 1] == '/')
				fullFileName.append(deviceID).append(".xml");
			inventory.Save(deviceID.c_str(), fullFileName.c_str());
		} else
			std::cerr << "No path/filename specified." << std::endl;
	} else {
		unsigned int waitSeconds = ::strtoul(
			config->KeyValue("waittime").c_str(), NULL, 10);

		std::cerr << "Waiting " << waitSeconds << " seconds..." << std::endl;
		::sleep(waitSeconds);

		inventory.Send(config->ServerURL().c_str());
	}
}
