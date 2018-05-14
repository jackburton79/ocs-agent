/*
 * LoggedUsers.h
 *
 *  Created on: 15/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef LOGGEDUSERS_H_
#define LOGGEDUSERS_H_

#include <ctime>
#include <string>

#include "ItemsList.h"

struct user_entry {
	std::string login;
	std::string logintimestring;
	time_t logintime;
};

class LoggedUsers : public ItemsList<user_entry> {
public:
	LoggedUsers();

};

#endif /* LOGGEDUSERS_H_ */
