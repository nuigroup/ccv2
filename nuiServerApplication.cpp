#include "nuiServerApplication.h"

LOG_DECLARE("CCV");

static bool config_syslog = false;
int g_config_delay = 200;

static void signal_term(int signal) 
{
	nuiJsonRpcApi::getInstance()->stopApi(true);
}

//! Entrance point
int main(int argc, char **argv) 
{
	int exit_ret = 0;
	// Initialize Signals
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	// Initialize Debug
	nuiDebugLogger::init(config_syslog);

	// Initialize JSON Configuration
	nuiFrameworkManagerErrorCode loadCode = nuiFrameworkManager::getInstance()->loadSettingsFromJson("data/default_config.json");
	if(loadCode != NUI_FRAMEWORK_MANAGER_OK){
		LOG(NUI_CRITICAL, "Failed to Initialize Configuration"); 
		return loadCode;
	}

	// Initialize JSON RPC 
	if(!nuiJsonRpcApi::getInstance()->init("127.0.0.1", 7500)) {
		LOG(NUI_CRITICAL, "Failed to Initialize JSON RPC");
		goto exit_critical;
	} else nuiJsonRpcApi::getInstance()->startApi();
	
		
	// Initialize Framework
	nuiFrameworkManagerErrorCode frameworkInitStatus = nuiFrameworkManager::getInstance()->initializeFrameworkManager();
	
	if(frameworkInitStatus != NUI_FRAMEWORK_MANAGER_OK) {
		if(frameworkInitStatus == NUI_FRAMEWORK_ROOT_INITIALIZATION_FAILED) 
			LOG(NUI_CRITICAL, "Failed to Initialize framework root");
		//if(frameworkInitStatus == NUI_FRAMEWORK_WRONG_FILE)
			//LOG(NUI_CRITICAL, "Incorrect File");
	} else nuiFrameworkManager::getInstance()->workflowStart();

	do {
		SLEEP(g_config_delay);
	} while ( nuiJsonRpcApi::getInstance()->isFinished() == false );
	

	// Stop Framework
	nuiFrameworkManager::getInstance()->workflowStop();
	nuiFrameworkManager::getInstance()->workflowQuit();

	exit_standard:
		// No cleanup required - Thread is done.
		return exit_ret;

	exit_critical:
		exit_ret = 1;
		goto exit_standard;
}