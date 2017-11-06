/*
 * main.cpp
 *
 *  Created on: 11/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Agent.h"
#include "Configuration.h"
#include "Logger.h"

#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include <string>

extern const char* __progname;
const char* version = "1.6.2";


static
struct option sLongOptions[] = {
		{ "conf", required_argument, 0, 'c' },
		{ "server", required_argument, 0, 's' },
		{ "local", required_argument, 0, 'l' },
		{ "stdout", no_argument, 0, 0 },
		{ "tag", required_argument, 0, 't' },
		{ "nosoftware", no_argument, 0, 0 },
		{ "daemonize", no_argument, 0, 'd' },
		{ "wait", required_argument, 0, 'w' },
		{ "help", no_argument, 0, 'h' },
		{ "verbose", no_argument, 0, 'v' },
		{ 0, 0, 0, 0 }
};


static void
PrintHelpAndExit()
{
	std::cout << __progname << " " << version << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "  -h, --help                         Print usage" << std::endl;
	std::cout << "  -c, --conf <config_file>           Specify configuration file" << std::endl;
	std::cout << "  -s, --server <server>              Specify OCSInventory server url" << std::endl;
	std::cout << "                                     If the server needs authentication, use the standard syntax <user>:<password>@<host>" << std::endl;
	std::cout << "  -l, --local <folder>               Don't send inventory, instead save a local copy in the specified file or folder" << std::endl;
	std::cout << "      --stdout                       Don't send inventory, print it to stdout" << std::endl;
	std::cout << "  -t, --tag <TAG>                    Specify tag. Will be ignored by server if a value already exists" << std::endl;
	std::cout << "      --nosoftware                   Do not retrieve installed software" << std::endl;
	std::cout << "  -D                                 DEPRECATED, use -d instead " << std::endl;
	std::cout << "  -d, --daemonize                    Detach from running terminal" << std::endl;
	std::cout << "  -w, --wait <s>                     Wait for the specified amount of seconds before contacting the server" << std::endl;
	std::cout << "  -v, --verbose                      Verbose mode" << std::endl;
	std::cout << std::endl;
	std::cout << "The -l and -s option are mutually exclusive." << std::endl;
	std::cout << "If no server or output file is specified, ";
	std::cout << "either via the -s/-l option or via configuration file (option -c), ";
	std::cout << "the program will exit without doing anything." << std::endl;
	std::cout << "Any warning/error will be written to the system logging facility. ";
	std::cout << "To print them to standard error, use the option '-v (--verbose)'." << std::endl;
	std::cout << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  Use the configuration file /etc/ocsinventory-ng.conf :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --conf /etc/ocsinventory-ng.conf" << std::endl;
	std::cout << std::endl;
	std::cout << "  Send inventory to server http://ocsinventory-ng/ocsinventory :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server http://ocsinventory-ng/ocsinventory" << std::endl;
	std::cout << std::endl;
	std::cout << "  Send inventory to server https://ocsinventory-ng/ocsinventory which requires http basic authentication :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --server https://user:password@ocsinventory-ng/ocsinventory" << std::endl;
	std::cout << std::endl;
	std::cout << "  Save a local inventory to /var/tmp/inventoryFile.xml :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --local /var/tmp/inventoryFile.xml" << std::endl;
	std::cout << std::endl;
	std::cout << "  Save a local inventory to /var/tmp/<device_id>.xml :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --local /var/tmp/" << std::endl;
	std::cout << std::endl;
	std::cout << "  Print inventory to standard output :" << std::endl;
	std::cout << "    " << __progname;
	std::cout << " --stdout" << std::endl;

	::exit(0);
}


int
main(int argc, char **argv)
{
	char* configFile = NULL;
	char* serverUrl = NULL;
	char* fullFileName = NULL;
	char* tag = NULL;
	int optIndex = 0;
	int c = 0;
	bool verbose = false;
	bool daemonize = false;
	
	Configuration* config = Configuration::Get();

	while ((c = ::getopt_long(argc, argv, "c:s:dDt:l:hvw:",
			sLongOptions, &optIndex)) != -1) {
		switch (c) {
			case 'c':
				configFile = optarg;
				break;
			case 's':
				serverUrl = optarg;
				break;
			case 'D':
			case 'd':
				daemonize = true;
				// TODO: Added this mostly for debugging. Without this,
				// Daemonize mode doesn't seem to work most of the time.
				config->SetVolatileKeyValue("waittime", "5");
				break;
			case 't':
				tag = optarg;
				break;
			case 'l':
				fullFileName = optarg;
				break;
			case 'h':
				PrintHelpAndExit();
				break;
			case 'v':
				verbose = true;
				break;
			case 'w':
				config->SetVolatileKeyValue("waittime", optarg);
				break;
			case 0:
				if (strcmp(sLongOptions[optIndex].name, "nosoftware") == 0)
					config->SetVolatileKeyValue("nosoftware", "true");
				else if (strcmp(sLongOptions[optIndex].name, "stdout") == 0 && !daemonize)
					config->SetVolatileKeyValue("stdout", "true");
				break;
		}
	}
	Logger& logger = Logger::GetDefault();
	if (verbose)
		logger.SetConsoleLogging(true);

	bool local = config->LocalInventory();
	bool stdout = config->KeyValue("stdout") == "true";
	if (!stdout && serverUrl == NULL
		&& configFile == NULL
		&& local && fullFileName == NULL) {
		PrintHelpAndExit();
	}

	if (daemonize) {
		pid_t processID = fork();
		if (processID < 0) {
			logger.Log(LOG_ERR, "Failed to daemonize. Exiting...");
			// Return failure in exit status
			exit(1);
		}

		// Exit the parent process
		if (processID > 0)
			exit(0);

		umask(0);
		if (chdir("/") < 0)
			; // Ignore

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
	else if (serverUrl != NULL)
		Configuration::Get()->SetServer(serverUrl);
	else if (fullFileName != NULL)
		Configuration::Get()->SetOutputFileName(fullFileName);

	if (tag != NULL)
		Configuration::Get()->SetVolatileKeyValue("TAG", tag);

	try {
		Agent agent;
		agent.Run();
	} catch (std::string& errorString) {
		logger.Log(LOG_ERR, errorString.c_str());
		return 1;
	} catch (const char* string) {
		logger.Log(LOG_ERR, string);
		return 1;
	} catch (int error) {
		logger.Log(LOG_ERR, strerror(error));
		return 1;
	} catch (std::exception& ex) {
		logger.Log(LOG_ERR, ex.what());
		return 1;
	} catch (...) {
		logger.Log(LOG_ERR, "Unhandled exception.");
		return 1;
	}

	if (configFile != NULL)
		Configuration::Get()->Save();

	return 0;
}

