#include "ccv.h"

LOG_DECLARE("App");

static bool config_syslog = false;
int g_config_delay = 20;

static void signal_term(int signal) 
{
	printf("ahhhhhhhh");
	nuiJsonRpcApi::getInstance()->stopApi();
}

int main(int argc, char **argv) 
{
	int exit_ret = 0;

	// initialize all signals
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	// initialize log
	nuiDebugLogger::init(config_syslog);

	// initialize daemon (network...)
	nuiDaemon::init();
	
	nuiFrameworkManager::getInstance()->loadAddonsAtPath("modules");
	nuiFrameworkManager::getInstance()->initializeFrameworkManager("configs/presets/test.xml");

	if(!nuiJsonRpcApi::getInstance()->init("127.0.0.1", 7500)) goto exit_critical;

	nuiJsonRpcApi::getInstance()->startApi();

	nuiFrameworkManager::getInstance()->workflowStart();

do
    {
		SLEEP(g_config_delay);
	} while ( nuiJsonRpcApi::getInstance()->isFinished() == false );

	nuiFrameworkManager::getInstance()->workflowStop();
	nuiFrameworkManager::getInstance()->workflowQuit();

exit_standard:
	// no server cleanup needed, done in thread
	return exit_ret;

exit_critical:
	exit_ret = 1;
	goto exit_standard;
}