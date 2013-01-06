#ifndef NUI_FACTORY_H
#define NUI_FACTORY_H

#include <vector>
#include <map>
#include <string>
#include <list>

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiPluginManager.h"

//! TODO: rework singleton
static nuiFactory *instance = NULL;

/** \class nuiFactory
 *  Singleton class. Provides methods for plugin creation and removal
 *  Also allows to get descriptors for already created plugins
 */
class nuiFactory 
{
public:
    //! Singleton wrapper
    static nuiFactory *getInstance();

    //! Function for factory removal
    static void cleanup();

    //! Forces PluginManager to load plugins from loaded dlls and obtains descriptors
    void loadDynamicModules();

    //! lists available pipeline names
    std::vector<std::string> *listPipelineNames();
    //! lists available module names
    std::vector<std::string> *listModuleNames();
    //! lists available modules and pipelines
    std::vector<std::string> *listAllNames();

    nuiModuleDescriptor* getDescriptor(const std::string &name);
    nuiModuleDescriptor* getDescriptor(const std::string &pipelineName, int id);

private:
    nuiFactory();

    std::map<std::string, nuiModuleDescriptor*> pipelineDescriptors;
    std::map<std::string, nuiModuleDescriptor*> moduleDescriptors;

    //! Set module instance params just like in descriptor
    void loadSettings(nuiModule* module, nuiModuleDescriptor* descriptor);

    //! Creates pipeline or module with specified name
    nuiModule *create(const std::string &name);
    //! Creates pipeline given pipeline descriptor
    nuiModule *createPipeline(nuiModuleDescriptor* descriptor);

    friend class nuiFrameworkManager;
};

#endif //NUI_FACTORY_H