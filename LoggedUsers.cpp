/*
 * LoggedUsers.cpp
 *
 *  Created on: 15/lug/2013
 *      Author: stefano
 */

#include "LoggedUsers.h"
#include "Support.h"


#include <istream>

LoggedUsers::LoggedUsers()
{
	if (!CommandExists("who"))
		throw "Missing command \"who\"";

	popen_streambuf who("export LC_ALL=C; who", "r");
	std::istream iStream(&who);

	std::string string;
	while (std::getline(iStream, string) > 0) {
		size_t pos = string.find(" ");
		if (pos != std::string::npos) {
			string = string.substr(0, pos);
			fUsers.push_back(string);
		}
	}
}


LoggedUsers::~LoggedUsers()
{
}


int
LoggedUsers::Count() const
{
	return fUsers.size();
}


std::string
LoggedUsers::UserAt(int i) const
{
	return fUsers[i];
}
