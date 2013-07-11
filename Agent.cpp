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
	// TODO Auto-generated destructor stub
}


void
Agent::Run()
{
	Inventory* inventory = new Inventory();
	inventory->Build();
	inventory->Print();

	if (false)
		inventory->Send();
}

