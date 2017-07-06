/*
 * Logger.h
 *
 *  Created on: 05/lug/2017
 *      Author: Stefano Ceccherini
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <syslog.h>

class Logger {
public:
	Logger(const char* logName);
	~Logger();

	void Log(int level, const char* const string);
	void LogFormat(int level, const char* string, ...);

	void SetConsoleLogging(bool value);

	static Logger& GetDefault();

private:
	bool fVerbose;
};

#endif /* LOGGER_H_ */
