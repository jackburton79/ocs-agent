/*
 * Logger.cpp
 *
 *  Created on: 05/lug/2017
 *      Author: Stefano Ceccherini
 */

#include "Logger.h"


#include <cstdio>
#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

extern const char* __progname;

static Logger* sDefaultLogger;

class StdErrLogger : public Logger {
public:
	StdErrLogger(const char* logName);
	virtual ~StdErrLogger();

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


Logger::Logger(const char* logName)
	:
	fLevel(LOG_INFO)
{
}


Logger::~Logger()
{
}


void
Logger::SetLevel(int level)
{
	fLevel = level;
}


void
Logger::Log(int level, const char* const string)
{
	if (level > fLevel)
		return;

	DoLog(level, string);
}


void
Logger::LogFormat(int level, const char* fmtString, ...)
{
	if (level > fLevel)
		return;

	char logString[1024];
	va_list argp;
	va_start(argp, fmtString);
	vsnprintf(logString, sizeof(logString), fmtString, argp);
	va_end(argp);
	DoLog(level, logString);
}


/* static */
Logger&
Logger::GetDefault()
{
	return Get(LOGGER_TYPE_DEFAULT);
}


/* static */
Logger&
Logger::Get(int loggerType)
{
	if (sDefaultLogger == NULL) {
		switch (loggerType) {
			case LOGGER_TYPE_SYSLOG:
				sDefaultLogger = new SyslogLogger(__progname);
				break;
			case LOGGER_TYPE_STDERR:
				sDefaultLogger = new StdErrLogger(__progname);
				break;
			case LOGGER_TYPE_DEFAULT:
			default:
				if (::isatty(STDIN_FILENO))
					sDefaultLogger = new StdErrLogger(__progname);
				else
					sDefaultLogger = new SyslogLogger(__progname);
				break;
		}
	}

	return *sDefaultLogger;
}


/* static */
Logger&
Logger::Get(const std::string& loggerType)
{
	if (::strcasecmp(loggerType.c_str(), "STDERR") == 0)
		return Get(Logger::LOGGER_TYPE_STDERR);
	else if (::strcasecmp(loggerType.c_str(), "SYSLOG") == 0)
		return Get(Logger::LOGGER_TYPE_SYSLOG);
	else
		return Get(Logger::LOGGER_TYPE_DEFAULT);
}


// StdoutLogger
StdErrLogger::StdErrLogger(const char* logName)
	:
	Logger(logName)
{
}

/* virtual */
StdErrLogger::~StdErrLogger()
{
}


/* virtual */
void
StdErrLogger::DoLog(int level, const char* string)
{
	std::cerr << string << std::endl;
}


// SyslogLogger
SyslogLogger::SyslogLogger(const char* logName)
	:
	Logger(logName)
{
	::openlog(logName, LOG_PID|LOG_CONS, LOG_USER);
}


/* virtual */
SyslogLogger::~SyslogLogger()
{
	::closelog();
}


/* virtual */
void
SyslogLogger::DoLog(int level, const char* string)
{
	::syslog(level, "%s", (const char* const)string);
}
