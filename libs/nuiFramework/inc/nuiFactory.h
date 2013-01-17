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

#ifdef WIN32
#include <guiddef.h>
#else
//! \todo include for GUID
#endif

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

    //! lists available pipelines GUID-pipeline_name
    std::vector<std::pair<GUID, std::string>>& listPipelines();
    //! lists available modules GUID-module_name
    std::vector<std::pair<GUID, std::string>>& listModules();

    //! gets descriptor for a module or pipeline with specified GUID
    nuiModuleDescriptor* getDescriptor(const GUID& guid);

    //! Set module instance params just like in descriptor
    void applyDescriptor(nuiModule* module, nuiModuleDescriptor* descriptor);

    //! Creates pipeline or module with specified GUID
    nuiModule* create(const GUID& guid);

private:
    nuiFactory();
    nuiFactory(const nuiFactory&);

    //! Creates pipeline given pipeline descriptor
    nuiModule* createPipeline(nuiModuleDescriptor* descriptor);
    //! Creates module given module descriptor
    nuiModule* createModule(nuiModuleDescriptor* descriptor);

    //! pipeline Descriptors
    //std::map<std::string, nuiModuleDescriptor*> pipelineDescriptors;
    // should be obtained directly from container (either Factory or PluginManager)

    //! module Descriptors
    //std::map<std::string, nuiModuleDescriptor*> moduleDescriptors;
    // should be obtained directly from container (PluginManager)

    // friend class nuiFrameworkManager;
};

#endif //NUI_FACTORY_H