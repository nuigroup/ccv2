/** 
 * \file      nuiFactory.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

/**
 * \mainpage NUI Group: Community Core Fusion (Community Core Vision 2)
 * ![Header Image](http://nuigroup.com/create/960-180-111111-ccv2)
 * \section intro Introduction
 * 
 * Introduction goes here
 * 
 * \section description Description
 * 
 * Description of the project here
 *
 * \section installation Installation
 * 
 * \subsection step1 Step 1
 * 
 * Step 1 goes here
 * 
 * \subsection step2 Step 2
 * 
 * Step 2 goes here
 */
#ifndef CCV_H
#define CCV_H

#include <stdio.h>
#define _WINSOCKAPI_
#ifdef WIN32
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

#else
#include <getopt.h>
#endif

#include <signal.h>

#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

// JSON
#include "cJSON.h"
#include "json/json.h"

// NUI
#include "nuiFramework.h"
#include "nuiTree.h"
#include "nuiJsonRpcApi.h"

// assert
#include <assert.h>
#include "nuiFrameworkManager.h"

#ifdef WIN32
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#endif