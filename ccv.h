#include <stdio.h>

#ifdef WIN32
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <Xgetopt.h>
#else
#include <getopt.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
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

// libevent
#include "event.h"
#include "evhttp.h"

// JSON
#include "cJSON.h"

// NUI
#include "nuiFramework.h"

#include "nuiTree.h"

#include "nuiJsonRpcApi.h"

// assert
#include <assert.h>
#include "../../libs/nuiFramework/inc/nuiFrameworkManager.h"

#ifdef WIN32
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif
