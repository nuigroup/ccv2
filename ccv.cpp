#include "ccv.h"

LOG_DECLARE("CCV");

static bool config_syslog = false;
int g_config_delay = 200;

static void signal_term(int signal) 
{
	LOG(NUI_TRACE, "ahhhhhhhh");
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

	nuiFrameworkManager::getInstance()->loadAddonsAtPath("modules");

	// initialize JSON RPC daemon and network
	if(!nuiJsonRpcApi::getInstance()->init("127.0.0.1", 7500)) goto exit_critical;

	//bool frameworkInitStatus = nuiFrameworkManager::getInstance()->init();
	bool frameworkInitStatus = nuiFrameworkManager::getInstance()->initializeFrameworkManager("configs/presets/test.json");
	if(frameworkInitStatus != NUI_FRAMEWORK_MANAGER_OK)
		if(frameworkInitStatus == NUI_FRAMEWORK_ROOT_INITIALIZATION_FAILED) LOG(NUI_CRITICAL, "Failed to initialize framework root");

	nuiJsonRpcApi::getInstance()->startApi();

	//nuiFrameworkManager::getInstance()->workflowStart();



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