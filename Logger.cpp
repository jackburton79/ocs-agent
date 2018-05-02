/*
 * Logger.cpp
 *
 *  Created on: 05/lug/2017
 *      Author: Stefano Ceccherini
 */

#include "Logger.h"


#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

extern const char* __progname;


static Logger* sDefaultLogger;


Logger::Logger(const char* logName)
{
}


Logger::~Logger()
{
}


void
Logger::Log(int level, const char* const string)
{
	DoLog(level, string);
}


void
Logger::LogFormat(int level, const char* fmtString, ...)
{
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
	if (sDefaultLogger == NULL) {
		if (::isatty(STDIN_FILENO))
			sDefaultLogger = new StdoutLogger(__progname);
		else
			sDefaultLogger = new SyslogLogger(__progname);
	}

	return *sDefaultLogger;
}


// StdoutLogger
StdoutLogger::StdoutLogger(const char* logName)
	:
	Logger(logName)
{
}

/* virtual */
StdoutLogger::~StdoutLogger()
{
}


/* virtual */
void
StdoutLogger::DoLog(int level, const char* string)
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
