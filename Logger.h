/*
 * Logger.h
 *
 *  Created on: 05/lug/2017
 *      Author: Stefano Ceccherini
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <syslog.h>

class Logger {
public:
	enum LOGGER_TYPE {
		LOGGER_TYPE_DEFAULT = 0,
		LOGGER_TYPE_STDERR = 1,
		LOGGER_TYPE_SYSLOG = 2
	};

	static void SetLevel(int level);

	static void Log(int level, const char* const string);
	static void LogFormat(int level, const char* fmtString, ...);

	static void SetLogger(LOGGER_TYPE loggerType = LOGGER_TYPE_DEFAULT);
	static void SetLogger(const std::string& loggerType);
	
private:
	static void _DoLog(int level, const char* string);

	static LOGGER_TYPE sLogType;
	static int sLevel;
};

#endif /* LOGGER_H_ */
