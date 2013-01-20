/** 
 * \file      nuiPluginManager.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */
#ifndef NUI_PLUGIN_MANAGER_H
#define NUI_PLUGIN_MANAGER_H

#include "nuiPlugin.h"
#include "nuiPluginEntity.h"
#include "nuiDynamicLibrary.h"
#include "nuiModule.h"

#include "json\json.h"

#include <map>
#include <vector>
#include <string>

//! \def MAJOR_VERSION defines major version of service that loads plugins
#define MAJOR_VERSION 1
//! \def MINOR_VERSION defines minor version of service that loads plugins
#define MINOR_VERSION 0

class nuiFactory;
class nuiDynamicLibrary;

struct nuiPluginLoaded;
struct nuiModuleLoaded;

//! Facade. Holds information about loaded plugins
struct nuiPluginLoaded
{
  //! \todo Move to separate file and split with realization
  //! string plugin location. As 2 plugins can't have the same address we can 
  //! consider pluginAddress as unique id.
  std::string pluginPath;
  //! handle to loaded library. Stored to be able to unload it later
  void* handle;
  //! guids of modules, loaded from plugin
  std::vector<nuiModuleLoaded*> loadedModules;

  void loadModule(nuiModuleLoaded* module)
  {
    this->loadedModules.push_back(module);
  };

  void unloadModule(nuiModuleLoaded* module)
  {
    for(int i=this->loadedModules.size() - 1; i>=0; i--)
      if (this->loadedModules[i] == module)
        this->loadedModules.erase(this->loadedModules.begin() + i);
  };

  nuiPluginLoaded(nuiDynamicLibrary* lib)
  {
    this->pluginPath = lib->getPath();
    this->handle = lib->getHandle();
  };
};

//! Facade. Holds information about modules loaded from plugins
struct nuiModuleLoaded
{
  //! \todo Move to separate file and split with realization
  //! module Name. Should be uniq within application.
  std::string name;
  //! parent plugin
  nuiPluginLoaded* parentPlugin;
  //! allocate Module
  nuiAllocateFunc allocateF;
  //! destroy Module
  nuiDeallocateFunc deallocateF;
  //! get Module descriptor
  nuiGetDescriptorFunc getDescriptorF;
  //! created module instances
  std::vector<void*> instances;

  nuiModuleLoaded(const nuiRegisterModuleParameters* registerParams)
  {
    this->allocateF = registerParams->allocateFunc;
    this->deallocateF = registerParams->deallocateFunc;
    this->getDescriptorF = registerParams->getDescriptorFunc;
    this->name = registerParams->name;
    this->instances.clear();
  };

  //! registers parent plugin for self and self for parent plugin
  void setParentPlugin(nuiPluginLoaded* parentPlugin)
  {
    this->parentPlugin = parentPlugin;
    parentPlugin->loadModule(this);
  };

  //! unregister from parent plugin
  void unregisterParent()
  {
    if(! this->parentPlugin)
      return;
    else
      this->parentPlugin->unloadModule(this);
  };

  nuiModule* allocate(nuiObjectParameters* params)
  {
    if(this->allocateF)
    {
      nuiModule* module = (nuiModule*)this->allocateF(params); 
      this->instances.push_back(module);
      return module;
    }
    else
      return NULL;
  };

  void deallocate(nuiModule* module)
  {
    if(this->deallocateF)
    {
      std::vector<void*>::iterator it;
      //! \todo don't forget to test
      for( it = instances.begin(); it != instances.end() ; it++)
      {
        if(*it == module)
          instances.erase(it);
      }
      // deallocate previously allocated module
      this->deallocate(module);
    }
  };

  nuiModuleDescriptor* getDescriptor()
  {
    return this->getDescriptorF();
  }

  void clearInstances()
  {
    for(int i=instances.size()-1 ; i>=0; i--)
    {
      this->deallocate((nuiModule*)instances[i]);
    }
    instances.clear();
  };

  ~nuiModuleLoaded()
  {
    clearInstances();
  };
};

/** \class nuiPluginManager
*  \brief Used to store information about loaded dlls with plugins,
*  plugins, hold pointers to functions for plugin and dll allocation/deallocation
*/
class nuiPluginManager
{
public:
  //! gets instance of nuiPluginManager
  static nuiPluginManager& getInstance();

  /** \fn nuiPluginFrameworkErrorCode registerModule(const char *moduleName, const nuiRegisterModuleParameters *params)
  *  \brief Called by dynamic library and used for sending data about module to nuiPluginManager. 
  *  Can be called several times by each dll if it contains more than one module.
  *  \return result of this operation to dynamic library
  */
  static nuiPluginFrameworkErrorCode::err registerModule(const nuiRegisterModuleParameters *params);

//==============================================================================
  /*! \fn nuiPluginFrameworkErrorCode loadLibrary(const std::string &path)
  *  \brief load dynamic library with specific path and add to internal collection
  *  \return result of this operation
  */
  nuiPluginFrameworkErrorCode::err loadLibrary(const std::string path);

  //! unload specified library and delete all previously allocated instances and plugin infos stored in this library
  nuiPluginFrameworkErrorCode::err unloadLibrary(const std::string path);

  //! loads default configuration (basic modules and sample pipeline)
  nuiPluginFrameworkErrorCode::err loadDefaultConfiguration();

  //! loads specified pipelines. Currently only json is supported.
  nuiPluginFrameworkErrorCode::err loadPipelines(Json::Value& root);

  //! loads specified pipelines from file. Currently only json is supported.
  nuiModuleDescriptor* loadPipeline(Json::Value& root);

  //! unloads specified pipeline from dictionary
  nuiPluginFrameworkErrorCode::err unloadPipeline(const std::string& name);

  //! lists names of loaded modules
  std::vector<std::string>& listLoadedModules();

  //! lists names of loaded pipelines
  std::vector<std::string>& listLoadedPipelines();

  //! registers newly created descriptor
  nuiPluginFrameworkErrorCode::err registerPipeline(nuiModuleDescriptor* descr);

  //! gets descriptor for module or pipeline with specified name
  nuiModuleDescriptor* getDescriptor(const std::string name);

  //! gets control structure for loaded module
  nuiModuleLoaded* getLoadedModule(const std::string name);

private:
  nuiPluginManager();
  nuiPluginManager(const nuiPluginManager&);

  //! plugin manager shutdown logic
  nuiPluginFrameworkErrorCode::err shutdown();
  
  /** \fn bool validate(const nuiRegisterModuleParameters *params)
  *  checks whether registerPluginParams were filled correctly
  */
  static bool validate(const nuiRegisterModuleParameters *params);

  /** unload specified module and delete all previously allocated instances of this module
  *  \return result of this operation
  */
  nuiPluginFrameworkErrorCode::err unloadModule(std::string name);

  /** loads settings to module Descriptor from json
   */
  void parseDescriptor(nuiModuleDescriptor &moduleDescriptor, const Json::Value& root);

  nuiPluginFrameworkService pluginFrameworkService;
  //! loaded shared libraries
  std::vector<nuiPluginLoaded*> pluginsLoaded;
  //! loaded modules
  std::vector<nuiModuleLoaded*> modulesLoaded;
  //! loaded pipelines
  std::vector<nuiModuleDescriptor*> pipelinesLoaded;

  //! currently loading plugin
  nuiPluginLoaded* loadingPlugin;
};
#endif//NUI_PLUGIN_MANAGER_H