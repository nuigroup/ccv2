/** 
 * \file      nuiFactory.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */
#ifndef NUI_FACTORY_H
#define NUI_FACTORY_H

#include <vector>
#include <map>
#include <string>
#include <list>

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiPluginManager.h"

/** \class nuiFactory
 *  Singleton class. Provides methods for plugin creation and removal
 *  Also allows to get descriptors for already created plugins
 */
class nuiFactory 
{
public:
    //! Singleton wrapper
    static nuiFactory& getInstance();
    
    //! Initialize with PluginManager to obtain loaded modules
    void init(const nuiPluginManager* pm);

    //! lists available pipeline names
    std::vector<std::string>& listPipelines();
    //! lists available module names
    std::vector<std::string>& listModules();

    //! gets descriptor for a module or pipeline with specified name
    nuiModuleDescriptor* getDescriptor(const std::string& name);

    //! Set module instance params just like in descriptor
    void applyDescriptor(nuiModule* module, nuiModuleDescriptor* descriptor);

    //! Creates pipeline or module with specified name
    nuiModule* create(const std::string& moduleName);

private:
    nuiFactory();
    nuiFactory(const nuiFactory&);

    //! plugin manager to obtain loaded modules or plugins
    const nuiPluginManager* pm;

    //! Creates pipeline given pipeline descriptor
    nuiModule* createPipeline(nuiModuleDescriptor* descriptor);
    //! Creates module given module descriptor
    nuiModule* createModule(nuiModuleLoaded* module);

    // friend class nuiFrameworkManager;
};

#endif //NUI_FACTORY_H