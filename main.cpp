/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"

#include <iostream>

int main()
{
	try {
		Agent agent;
		agent.Run();
	} catch (...) {
		std::cerr << "Error." << std::endl;
		return 1;
	}
	return 0;
}

