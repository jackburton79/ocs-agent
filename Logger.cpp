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

extern const char* __progname;


static Logger sDefaultLogger(__progname);


Logger::Logger(const char* logName)
	:
	fVerbose(false)
{
	::openlog(logName, LOG_PID|LOG_CONS, LOG_USER);
}


Logger::~Logger()
{
	::closelog();
}


void
Logger::Log(int level, const char* const string)
{
	::syslog(level, string);
	if (fVerbose)
		std::cerr << string << std::endl;
}


void
Logger::LogFormat(int level, const char* fmtString, ...)
{
	char logString[1024];
	va_list argp;
	va_start(argp, fmtString);
	vsnprintf(logString, sizeof(logString), fmtString, argp);
	va_end(argp);
	::syslog(level, (const char* const)logString);
	if (fVerbose)
		std::cerr << logString << std::endl;
}


void
Logger::SetConsoleLogging(bool value)
{
	fVerbose = value;
}


/* static */
Logger&
Logger::GetDefault()
{
	return sDefaultLogger;
}

