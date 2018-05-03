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
	enum LOGGER_TYPE {
		LOGGER_TYPE_DEFAULT = 0,
		LOGGER_TYPE_STDERR = 1,
		LOGGER_TYPE_SYSLOG = 2
	};

	Logger(const char* logName);
	virtual ~Logger();

	void SetLevel(int level);

	void Log(int level, const char* const string);
	void LogFormat(int level, const char* string, ...);

	static Logger& GetDefault();
	static Logger& Get(int loggerType = LOGGER_TYPE_DEFAULT);
	
private:
	virtual void DoLog(int level, const char* string) = 0;

	int fLevel;
};


class StdoutLogger : public Logger {
public:
	StdoutLogger(const char* logName);
	virtual ~StdoutLogger();

private:
	virtual void DoLog(int level, const char* string);
};


class SyslogLogger : public Logger {
public:
	SyslogLogger(const char* logName);
	virtual ~SyslogLogger();

private:
	virtual void DoLog(int level, const char* string);
};

#endif /* LOGGER_H_ */
