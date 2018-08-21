/*
 * LoggedUsers.h
 *
 *  Created on: 15/lug/2013
 *      Author: Stefano Ceccherini
 */

#ifndef USERSROSTER_H_
#define USERSROSTER_H_

#include <ctime>
#include <string>

#include "ItemsList.h"

struct user_entry {
	std::string login;
	std::string logintimestring;
	time_t logintime;
};

class UsersRoster : public ItemsList<user_entry> {
public:
	UsersRoster();

};

#endif /* USERSROSTER_H_ */
