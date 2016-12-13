/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Agent.h"
#include "Configuration.h"

#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>


extern const char* __progname;

static
struct option sLongOptions[] = {
		{ "conf", required_argument, 0, 'c' },
		{ "server", required_argument, 0, 's' },
		{ "help", no_argument, 0, 'h' },
		{ 0, 0, 0, 0 }
};


static void
PrintHelpAndExit()
{
	std::cout << __progname << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "-h [--help]         : Print usage" << std::endl;
	std::cout << "-c [--conf]         : Specify configuration file" << std::endl;
	std::cout << "-s [--server]       : Specify OCSInventory server url" << std::endl;
	std::cout << "If no server is specified, either via the -s option or via the" << std::endl;
	std::cout << "configuration file (option -c), the program will write a local" << std::endl;
	std::cout << "inventory in the current working directory." << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --conf /etc/ocsinventory-ng.conf" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server http://ocsinventory-ng/ocsinventory" << std::endl;

	::exit(0);
}


int
main(int argc, char **argv)
{
	char* configFile = NULL;
	char* serverUrl = NULL;
	int optIndex = 0;
	int c = 0;
	while ((c = ::getopt_long(argc, argv, "c:s:h",
			sLongOptions, &optIndex)) != -1) {
		switch (c) {
			case 'c':
				configFile = optarg;
				break;
			case 's':
				serverUrl = optarg;
				break;
			case 'h':
				PrintHelpAndExit();
				break;
		}
	}


	if (configFile != NULL)
		Configuration::Get()->Load(configFile);
	else if (serverUrl)
		Configuration::Get()->SetServer(serverUrl);

	try {
		Agent agent;
		agent.Run();
	} catch (std::string& errorString) {
		std::cerr << errorString << std::endl;
		return 1;
	} catch (const char* string) {
		std::cerr << string << std::endl;
		return 1;
	} catch (int error) {
		std::cerr << strerror(error) << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception." << std::endl;
		return 1;
	}

	if (configFile != NULL)
		Configuration::Get()->Save();

	return 0;
}

