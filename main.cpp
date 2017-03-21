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
#include <unistd.h>
#include <sys/stat.h>

extern const char* __progname;

static
struct option sLongOptions[] = {
		{ "conf", required_argument, 0, 'c' },
		{ "server", required_argument, 0, 's' },
		{ "output", required_argument, 0, 'o' },
		{ "daemonize", no_argument, 0, 'D' },
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
	std::cout << "-o [--output]       : Specify output file name" << std::endl;
	std::cout << "-D [--daemonize]    : Detach from running terminal" << std::endl;
	std::cout << "The -o and -s option are mutually exclusive. If no server or output file is specified,";
	std::cout << "either via the -s/-o option or via configuration file (option -c),";
	std::cout << "the program will exit without doing anything." << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --conf /etc/ocsinventory-ng.conf" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server http://ocsinventory-ng/ocsinventory" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --output /path/to/output/inventoryFile.xml" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --output /path/to/output/" << std::endl;
	

	::exit(0);
}


int
main(int argc, char **argv)
{
	char* configFile = NULL;
	char* serverUrl = NULL;
	char* fullFileName = NULL;
	int optIndex = 0;
	int c = 0;
	bool daemonize = false;
	while ((c = ::getopt_long(argc, argv, "c:s:Do:h",
			sLongOptions, &optIndex)) != -1) {
		switch (c) {
			case 'c':
				configFile = optarg;
				break;
			case 's':
				serverUrl = optarg;
				break;
			case 'D':
				daemonize = true;
				break;
			case 'o':
				fullFileName = optarg;
				break;
			case 'h':
				PrintHelpAndExit();
				break;
		}
	}

	if (daemonize) {
		pid_t processID = fork();
		if (processID < 0) {
			std::cerr << "Failed to daemonize. Exiting..." << std::endl;
			// Return failure in exit status
			exit(1);
		}

		// Exit the parent process
		if (processID > 0)
			exit(0);

		umask(0);

		//set new session
		pid_t sid = setsid();
		if (sid < 0)
			exit(1);

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	if (configFile != NULL)
		Configuration::Get()->Load(configFile);
	else if (serverUrl)
		Configuration::Get()->SetServer(serverUrl);
	else if (fullFileName)
		Configuration::Get()->SetOutputFileName(fullFileName);
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

