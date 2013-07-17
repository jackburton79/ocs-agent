/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"

#include <iostream>
#include <string.h>

int main()
{
	try {
		Agent agent;
		agent.Run();
	} catch (std::string errorString) {
		std::cerr << errorString << std::endl;
		return 1;
	} catch (int error) {
		std::cerr << strerror(error) << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception." << std::endl;
		return 1;
	}
	return 0;
}

