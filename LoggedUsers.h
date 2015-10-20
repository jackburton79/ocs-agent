/*
 * LoggedUsers.h
 *
 *  Created on: 15/lug/2013
 *      Author: stefano
 */

#ifndef LOGGEDUSERS_H_
#define LOGGEDUSERS_H_

#include <ctime>
#include <string>
#include <vector>

struct user_entry {
	std::string login;
	std::string logintimestring;
	time_t logintime;
};

class LoggedUsers {
public:
	LoggedUsers();
	~LoggedUsers();

	int Count() const;
	std::string LoginNameAt(int i) const;
	user_entry UserEntryAt(int i) const;

private:
	std::vector<user_entry> fUsers;
};

#endif /* LOGGEDUSERS_H_ */
