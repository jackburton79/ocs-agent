/*
 * LoggedUsers.h
 *
 *  Created on: 15/lug/2013
 *      Author: stefano
 */

#ifndef LOGGEDUSERS_H_
#define LOGGEDUSERS_H_

#include <string>
#include <vector>

class LoggedUsers {
public:
	LoggedUsers();
	~LoggedUsers();

	int Count() const;
	std::string UserAt(int i) const;

private:
	std::vector<std::string> fUsers;
};

#endif /* LOGGEDUSERS_H_ */
