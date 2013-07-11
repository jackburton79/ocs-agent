/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"

int main()
{
	try {
		Agent agent;
		agent.Run();
	} catch (...) {
		return 1;
	}
	return 0;
}

