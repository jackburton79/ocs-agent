/*
 * UNameBackend.cpp
 *
 *  Created on: 22 mar 2021
 *      Author: stefano
 */

#include "UnameBackend.h"

#include "Machine.h"

#include <cstring>
#include <errno.h>
#include <iostream>

#include <sys/utsname.h>


UnameBackend::UnameBackend()
{
}


UnameBackend::~UnameBackend()
{
}


/* virtual */
int
UnameBackend::Run()
{
	struct utsname uName;
	if (::uname(&uName) != 0) {
		std::string errorString = "OSInfo::_GetOsInfo(): uname() failed with error ";
		errorString.append(::strerror(errno));
		throw std::runtime_error(errorString);
	}

	Component osInfo;
	osInfo.fields["hostname"] = uName.nodename;
	osInfo.fields["comments"] = uName.version;
	osInfo.fields["release"] = uName.release;
	osInfo.fields["domainname"] = uName.domainname;
	osInfo.fields["architecture"] = uName.machine;

	std::cout << osInfo.fields["hostname"] << std::endl;
	std::cout << osInfo.fields["comments"] << std::endl;
	std::cout << osInfo.fields["release"] << std::endl;
	std::cout << osInfo.fields["domainname"] << std::endl;
	std::cout << osInfo.fields["architecture"] << std::endl;
	//Feed domain name from host name when possible.
	if (osInfo.fields["domainname"] == "" || osInfo.fields["domainname"] == "(none)") {
		size_t dotPos = osInfo.fields["hostname"].find('.');
		if (dotPos != std::string::npos) {
			osInfo.fields["domainname"] = osInfo.fields["hostname"].substr(dotPos + 1);
		}
	}

	gComponents["OS"].MergeWith(osInfo);

	return 0;
}
