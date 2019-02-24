/*
 * LoggedUsers.cpp
 *
 *  Created on: 15/lug/2013
 *      Author: Stefano Ceccherini
 */

#include "Support.h"

#include <ctime>

#include <utmpx.h>
#include <UsersRoster.h>

UsersRoster::UsersRoster()
{
	setutxent();

	struct utmpx* record = NULL;
	while ((record = getutxent()) != NULL) {
		if (record->ut_type == USER_PROCESS) {
			user_entry entry;
			entry.login = record->ut_user;
			time_t loginTime = record->ut_tv.tv_sec;
			entry.logintime = loginTime;
			struct tm timeInfoStruct;
			struct tm* timeinfo = localtime_r(&loginTime, &timeInfoStruct);
			char timeString[64];
			strftime(timeString, sizeof(timeString), "%a %b %d %R", timeinfo);
			entry.logintimestring = timeString;
			fItems.push_back(entry);
		}
	}

	endutxent();

	Rewind();
}
