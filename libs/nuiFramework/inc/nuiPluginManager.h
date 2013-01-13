/* nuiPluginManager.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef NUI_PLUGIN_MANAGER_H
#define NUI_PLUGIN_MANAGER_H

#include "nuiPlugin.h"
#include "nuiPluginEntity.h"
#include "nuiDynamicLibrary.h"

#include <map>
#include <vector>
#include <string>

#ifdef WIN32
#include <guiddef.h>
#else
//! \todo include headers for GUID
#endif

//! \def MAJOR_VERSION defines major version of service that loads plugins
#define MAJOR_VERSION 1
//! \def MINOR_VERSION defines minor version of service that loads plugins
#define MINOR_VERSION 0

class nuiFactory;
class nuiDynamicLibrary;

struct nuiPluginLoaded;
struct nuiModuleLoaded;

//! Holds information about loaded plugins
struct nuiPluginLoaded
{
  //! string plugin location. As 2 plugins can't have the same address we can 
  //! consider pluginAddress as unique id.
  std::string pluginPath;
  //! handle to loaded library. Stored to be able to unload it later
  void* handle;
  //! guids of modules, loaded from plugin
  std::vector<nuiModuleLoaded*> loadedModules;

  nuiPluginLoaded()
  {

  };

  void loadModule(nuiModuleLoaded* module)
  {
    loadedModules.push_back(module);
  };

  nuiPluginLoaded(nuiDynamicLibrary* lib)
  {
    this->pluginPath = lib->getPath();
    this->handle = lib->getHandle();
  };

  ~nuiPluginLoaded()
  {
  };
};

//! Holds information about modules loaded from plugins
struct nuiModuleLoaded
{
  //! unique module identifier within application
  GUID guid;
  //! parent plugin
  nuiPluginLoaded* parentPlugin;
  //! module Name
  char* name;
  //! allocate Module
  nuiAllocateFunc allocate;
  //! destroy Module
  nuiDeallocateFunc deallocate;
  //! get Module descriptor
  nuiGetDescriptorFunc getDescriptor;
  //! created module instances
  std::vector<void*> instances;

  nuiModuleLoaded()
  {
    this->instances.clear();
  };

  nuiModuleLoaded(const nuiRegisterModuleParameters* registerParams)
  {
    this->guid = registerParams->guid;
    this->allocate = registerParams->allocateFunc;
    this->deallocate = registerParams->deallocateFunc;
    this->getDescriptor = registerParams->getDescriptorFunc;
    this->instances.clear();
  };

  //! registers parent plugin for self and self for parent plugin
  void setParentPlugin(nuiPluginLoaded* parentPlugin)
  {
    this->parentPlugin = parentPlugin;
    parentPlugin->loadModule(this);
  };

  void clearInstances()
  {
    for(int i=instances.size()-1 ; i>=0; i--)
    {
      this->deallocate(instances[i]);
    }
    instances.clear();
  };

  ~nuiModuleLoaded()
  {
    clearInstances();
    this->instances.clear();
  };
};

/** \class nuiPluginManager
*  Singleton class. Used to store information about loaded dlls with plugins,
*  plugins, hold pointers to functions for plugin and dll allocation/deallocation
*/
class nuiPluginManager
{
public:
  //! gets instance of nuiPluginManager
  static nuiPluginManager& getInstance();

  /** \fn nuiPluginFrameworkErrorCode registerModule(const char *moduleName, const nuiRegisterModuleParameters *params)
  *  Called by dynamic library and used for sending data about module to nuiPluginManager. 
  *  Can be called several times by each dll if it contains more than one module.
  *  \return result of this operation to dynamic library
  */
  static nuiPluginFrameworkErrorCode::err registerModule(const nuiRegisterModuleParameters *params);

//==============================================================================
  /*! \fn nuiPluginFrameworkErrorCode loadLibrary(const std::string &path)
  *  load dynamic library with specific path and add to internal collection
  *  \return result of this operation
  */
  nuiPluginFrameworkErrorCode::err loadLibrary(const std::string path);

  /** unload specified library and delete all previously allocated instances 
  *  and plugin infos stored in this library
  *  \return result of this operation
  */
  nuiPluginFrameworkErrorCode::err unloadLibrary(const std::string path);

private:
  nuiPluginManager();

  //! plugin manager shutdown logic
  nuiPluginFrameworkErrorCode::err shutdown();
  
  /** \fn bool validate(const nuiRegisterModuleParameters *params)
  *  checks whether registerPluginParams were filled correctly
  */
  static bool validate(const nuiRegisterModuleParameters *params);

  /** unload specified module and delete all previously allocated instances of this module
  *  \return result of this operation
  */
  nuiPluginFrameworkErrorCode::err unloadModule(GUID guid);

  nuiPluginFrameworkService pluginFrameworkService;
  //! loaded shared libraries
  std::vector<nuiPluginLoaded*> pluginsLoaded;
  //! loaded modules
  std::vector<nuiModuleLoaded*> modulesLoaded;
  
  //last id assigned to module
  //int assignedModuleId;
  //last id assigned to plugin
  //int assignedPluginId;

  //! currently loading plugin
  nuiPluginLoaded* loadingPlugin;
  //! modules being loaded from loadingPlugin
  // std::vector<nuiModuleLoaded*> loadingModules;
};
#endif//NUI_PLUGIN_MANAGER_H