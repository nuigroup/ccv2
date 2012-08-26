#ifndef JSONTCPTEST_H 
#define JSONTCPTEST_H 

#include <stdio.h>
#include <cstdio>

#ifdef WIN32
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#include <Winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <Xgetopt.h>
#else
#include <getopt.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <iostream>
#include <string>

#include "json/json.h"
#include "networking.h"
#include "jsonrpc.h"
#include "jsonrpc_tcpclient.h"


static Json::Rpc::TcpClient *client;

#endif /* JSONTCPTEST_H */