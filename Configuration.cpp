/*
 * Configuration.cpp
 *
 *  Created on: 13/lug/2013
 *      Author: stefano
 */

#include "Configuration.h"

Configuration::Configuration()
{
}


Configuration::~Configuration()
{
}


std::string
Configuration::ServerURL() const
{
	// TODO: Make this non-fixed.
	return "http://ocsinventory-ng";
}


bool
Configuration::LocalInventory() const
{
	return true;
}

