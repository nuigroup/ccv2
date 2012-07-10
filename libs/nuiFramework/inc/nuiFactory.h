#ifndef NUI_FACTORY_H
#define NUI_FACTORY_H

#include <vector>
#include <map>
#include <string>
#include <list>
#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiPluginManager.h"

using namespace nuiPluginFramework;

class nuiFactory 
{
public:
	static nuiFactory *getInstance();
	~nuiFactory();
	void loadDynamicModules(char *pathLocation);
	void init();
	static void cleanup();
	std::vector<std::string> *listPipelineNames();
	std::vector<std::string> *listModuleNames();
	std::vector<std::string> *listAllNames();	
	nuiModuleDescriptor* getDescriptor(const std::string &name);
	nuiModuleDescriptor* getDescriptor(const std::string &pipelineName,int id);
private:
	nuiModule *create(const std::string &name);
	void remove(nuiModule *module);
	nuiModule *createPipeline(nuiModuleDescriptor* descriptor);
	void loadSettings(nuiModule* module, nuiModuleDescriptor* descriptor);
private:
	nuiFactory();
	std::map<std::string, nuiModuleDescriptor*> pipelineDescriptors;
	std::map<std::string, nuiModuleDescriptor*> moduleDescriptors;
	friend class nuiFrameworkManager;
};

#endif

