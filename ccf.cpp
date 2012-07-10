#include "ccf.h"

LOG_DECLARE("App");

static bool want_quit = false;
static bool config_syslog = false;
static nuiJsonRpcApi *server = NULL;
int g_config_delay = 20;

bool want_quit_soon = false;

static void signal_term(int signal) 
{
	want_quit = true;
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
	
	nuiFrameworkManager::getInstance()->loadAddonsAtPath("addons");
	nuiFrameworkManager::getInstance()->initializeFrameworkManager("configs/presets/test.xml");
	nuiFrameworkManager::getInstance()->workflowStart();

	nuiJsonRpcApi::getInstance()->init("127.0.0.1", 7500);

	server = nuiJsonRpcApi::getInstance();
	server->startApi();

	nuiTreeNode<int, int>* node = new nuiTreeNode<int, int>(0,0);
	node->addChildNode(new nuiTreeNode<int, int>(1,1));
	node->addChildNode(new nuiTreeNode<int, int>(2,2));
	node->addChildNode(new nuiTreeNode<int, int>(3,3));
	node->getChild(1)->addChildNode(new nuiTreeNode<int, int>(4,4));
	node->getChild(1)->addChildNode(new nuiTreeNode<int, int>(5,5));
	node->getChild(3)->addChildNode(new nuiTreeNode<int, int>(6,6));
	node->getChild(3)->addChildNode(new nuiTreeNode<int, int>(7,7));
	nuiTree<int,int>* tree = new nuiTree<int,int>(node);

	for (nuiTree<int,int>::iterator iter = tree->begin();iter!=tree->end();iter++)
	{
		printf("%i ", (*(iter))->getKey());
	}


do
    {
		SLEEP(g_config_delay);
	} while ( server->isFinished() == false );

	nuiFrameworkManager::getInstance()->workflowStop();
	nuiFrameworkManager::getInstance()->workflowQuit();

exit_standard:
	// no server cleanup needed, done in thread
	return exit_ret;

exit_critical:
	exit_ret = 1;
	goto exit_standard;
}