#ifndef _CCV_H
#define _CCV_H

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