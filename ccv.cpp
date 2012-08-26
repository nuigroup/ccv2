#include "ccv.h"

LOG_DECLARE("CCV");

static bool config_syslog = false;
int g_config_delay = 200;

static void signal_term(int signal) 
{
	nuiJsonRpcApi::getInstance()->stopApi(true);
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

	// PSUEDO
	//nuiFrameworkManager::getInstance()->loadJSONConfigData("data/default_config.json");
	//nuiFrameworkManager::getInstance()->loadModulesFromJSON(JSON_OBJECT);

	// FIRST LOAD THE JSON
	nuiFrameworkManagerErrorCode loadCode = nuiFrameworkManager::getInstance()->loadSettingsFromJson("data/default_config.json");
	if(loadCode != NUI_FRAMEWORK_MANAGER_OK) return loadCode;

	// REPLACE THIS WITH JSON DATA
	// nuiFrameworkManager::getInstance()->loadAddonsAtPath("modules");
	
	 nuiFrameworkManager::getInstance()->loadAddonsAtPath("modules");

	// START JSON RPC API
	if(!nuiJsonRpcApi::getInstance()->init("127.0.0.1", 7500)) goto exit_critical;
	nuiJsonRpcApi::getInstance()->startApi();
	
	// INITIALIZE THE FRAMEWORK
	nuiFrameworkManagerErrorCode frameworkInitStatus = nuiFrameworkManager::getInstance()->initializeFrameworkManager(""); // TODO: pass this loadCode
	

	// OLD...
	//bool frameworkInitStatus = nuiFrameworkManager::getInstance()->init();
	//

	
	
	if(frameworkInitStatus != NUI_FRAMEWORK_MANAGER_OK) {
		if(frameworkInitStatus == NUI_FRAMEWORK_ROOT_INITIALIZATION_FAILED) 
			LOG(NUI_CRITICAL, "Failed to initialize framework root");
		if(frameworkInitStatus == NUI_FRAMEWORK_WRONG_FILE)
			LOG(NUI_CRITICAL, "Wrong file");
	} else nuiFrameworkManager::getInstance()->workflowStart();

	do {
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