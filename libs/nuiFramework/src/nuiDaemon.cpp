/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDaemon.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include <iostream>
#include <fstream>

#include "nuiPipeline.h"
#include "nuiModule.h"
#include "nuiFactory.h"
#include "nuiProperty.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"

#include "nuiDaemon.h"
#include "nuiDebugLogger.h"
#include "nuiFactory.h"

#ifdef WIN32
#include <windows.h>
#endif

LOG_DECLARE("Daemon");

void nuiDaemon::init() {
	nuiFactory::getInstance()->init();
	
#ifdef WIN32
	// initialize network for Win32 platform
	{
		WSADATA wsaData;
		if ( WSAStartup(MAKEWORD(2, 2), &wsaData) == -1 )
			LOG(NUI_CRITICAL, "unable to initialize WinSock (v2.2)");
	}
#endif
	
}

bool nuiDaemon::detach(std::string pidfilename) {
#ifndef WIN32
	pid_t pid = fork();
	if (pid > 0) {
		LOG(NUI_INFO, "child process created with pid " << pid);
		try {
			std::ofstream pidfile(pidfilename.c_str(), std::ios::out|std::ios::trunc);
			if (pidfile) {
				pidfile << pid << std::endl;
				pidfile.close();
			} else {
				LOG(NUI_ERROR, "Cannot write pidfile " << pidfilename);
			}
		} 
		catch(std::exception x) {
			LOG(NUI_ERROR, "Cannot write pidfile " << pidfilename << ": " << x.what());
		}
	}
	if (pid < 0)
		LOG(NUI_ERROR, "no child process could be created, but this process is still living");
	return(pid <= 0);
#endif
	// TODO implement other platform
	LOG(NUI_INFO, "This platform don't support detach yet.");
	
	return true;
}

void nuiDaemon::cleanup() {
	
#ifdef _WIN32
	WSACleanup();
#endif

	nuiFactory::getInstance()->cleanup();
	nuiDebugLogger::cleanup();
	
}

