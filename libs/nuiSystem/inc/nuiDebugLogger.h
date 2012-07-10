/* nuiDebugLogger.h
*  
*  Created on 02/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/

#ifndef _NUI_DEBUG_LOGGER_
#define _NUI_DEBUG_LOGGER_

#include <sstream>
#include <iostream>
#include <string>

#ifdef NO_LOG

#define LOG_DECLARE(x)
#define LOG(level, x)
#define LOGX(level, x)
#define LOGM(level, x)

#else // NO_LOG

#define LOG_DECLARE(x) \
	static char log_name[] = x;

#define LOG(level, x) do { \
	if ( level <= g_loglevel ) \
		nuiLoggerMessage(log_name, __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGX(level, x) do { \
	if ( level <= g_loglevel ) \
		nuiLoggerMessage(this->getName(), __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGM(level, x) do { \
	if ( level <= g_loglevel ) \
		nuiLoggerMessage(log_name, __FILE__, __LINE__, level) \
		<< "<" << this->property("id").asString() << "> " << x; \
} while(0);

#endif // NO_LOG

#define _LOG_FUNC { \
	if ( this->level <= g_loglevel ) \
		this->os << __n; \
	return *this; \
}

extern int g_loglevel;

enum {
	NUI_CRITICAL	= 0,
	NUI_ERROR		= 1,
	NUI_WARNING		= 2,
	NUI_INFO		= 3,
	NUI_DEBUG		= 4,
	NUI_TRACE		= 5,
};

class nuiDebugLogger {
public:
	static void init(bool use_syslog);
	static void cleanup();
	static void setLogLevel(int n);
	static int getLogLevel();
	static int getSysLogLevel(int n);
	static std::string getLogLevelName(int n);
};

class nuiLoggerMessage {
public:
	nuiLoggerMessage(std::string name, std::string filename, int line, int level);
	~nuiLoggerMessage();
	template<typename T> nuiLoggerMessage &operator<<(T __n) _LOG_FUNC;
private:
	std::ostringstream os;
	int level;
};
#endif//_NUI_DEBUG_LOGGER_