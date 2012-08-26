#include <time.h>
#include <stdlib.h>

#include "nuiDebugLogger.h"
#include "pasync.h"

#ifndef WIN32
#include <syslog.h>
#endif

static pt::mutex(logmtx);

int g_loglevel = NUI_TRACE;
bool g_use_syslog = false;

nuiLoggerMessage::nuiLoggerMessage(std::string name, std::string filename, int line, int level) 
{
	logmtx.lock();
	char buffer[64];
	time_t t;
	struct tm *tmp;

	if (! g_use_syslog) {
		t = time(NULL);
		tmp = localtime(&t);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", tmp);

		this->os << buffer << " | ";
	}
	this->os << nuiDebugLogger::getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
	logmtx.unlock();
}

nuiLoggerMessage::~nuiLoggerMessage() 
{
	if (this->level <= g_loglevel) {
		logmtx.lock();
#ifndef WIN32
		if (g_use_syslog) {
			syslog(LOG_USER | nuiDebugLogger::getSysLogLevel(this->level), "%s", this->os.str().c_str());
		} else {
			std::cout << this->os.str() << std::endl;
		}
#else
		std::cout << this->os.str() << std::endl;
#endif
		logmtx.unlock();
	}
}


void nuiDebugLogger::init(bool use_syslog) 
{
#ifndef WIN32
	g_use_syslog = use_syslog;
#endif
	g_loglevel = NUI_TRACE;
	if (getenv("NUI_DEBUG"))
		g_loglevel = NUI_DEBUG;
	if (getenv("NUI_TRACE"))
		g_loglevel = NUI_TRACE;
}

void nuiDebugLogger::cleanup() 
{
}

int nuiDebugLogger::getLogLevel() 
{
	return g_loglevel;
}

void nuiDebugLogger::setLogLevel(int n) 
{
	g_loglevel = n;
}

int nuiDebugLogger::getSysLogLevel(int n) 
{
#ifndef WIN32
	switch (n) {
		case NUI_CRITICAL:	return LOG_CRIT;
		case NUI_ERROR:		return LOG_ERR;
		case NUI_WARNING:	return LOG_WARNING;
		case NUI_INFO:		return LOG_INFO;
		case NUI_DEBUG:		return LOG_DEBUG;
		//case NUI_TRACE:		return LOG_TRACE;
		default:		return 0;
	}
#endif
	// TODO implement system logging for Win32
	// perhaps we should use boost::log to abstract? but then System depends on Boost...
	return 0;
}

std::string nuiDebugLogger::getLogLevelName(int n) 
{
	switch (n) {
		case NUI_CRITICAL:	return "Critical";
		case NUI_ERROR:		return "Error";
		case NUI_WARNING:	return "Warning";
		case NUI_INFO:		return "Info";
		case NUI_DEBUG:		return "Debug";
		case NUI_TRACE:		return "Trace";
	}
	return "Unknown";
}
