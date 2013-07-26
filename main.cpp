/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: stefano
 */

#include "Agent.h"
#include "Configuration.h"

#include <iostream>
#include <string.h>
#include <getopt.h>

static
struct option sLongOptions[] = {
		{ "conf", required_argument, 0, 'c' },
		{ 0, 0, 0, 0 }
};

int
main(int argc, char **argv)
{
	char* configFile = NULL;
	int optIndex = 0;
	int c = 0;
	while ((c = ::getopt_long(argc, argv, "c:",
			sLongOptions, &optIndex)) != -1) {
		switch (c) {
			case 'c':
				configFile = optarg;
				break;
		}
	}

	if (configFile != NULL)
		Configuration::Get()->Load(configFile);

	try {
		Agent agent;
		agent.Run();
	} catch (std::string& errorString) {
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

