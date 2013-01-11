#include "nuiDynamicLibrary.h"
#include "nuiPluginManager.h"
#include <string>
#include <guiddef.h>

nuiPluginManager& nuiPluginManager::getInstance()
{
  static nuiPluginManager* instance = NULL;
  if(!instance)
    instance = new nuiPluginManager();
  return *instance;
}

nuiPluginManager::nuiPluginManager()
{
  pluginFrameworkService.version.major = MAJOR_VERSION;
  pluginFrameworkService.version.minor = MINOR_VERSION;
  pluginFrameworkService.registerModule = registerModule;

//   assignedPluginId = 0;
//   assignedModuleId = 0;

  loadingPlugin = NULL;
  loadingModules.clear();
}

//! \todo destructor realization needed?
// nuiPluginManager::~nuiPluginManager()
// {
//   shutdown();
// };

nuiPluginFrameworkErrorCode::err nuiPluginManager::shutdown()
{
  // 	nuiPluginFrameworkErrorCode result = nuiPluginFrameworkOK;
  //     nuiDynamicLibraryMap::iterator i;
  // 	for ( i = dynamicLibraryMap.begin(); i != dynamicLibraryMap.end(); i++)
  // 		unloadLibrary(i->second);
  //     nuiDynamicLibraryFreeVec::iterator func;
  // 
  // 	for (func = dynamicLibraryFreeVector.begin(); func != dynamicLibraryFreeVector.end(); ++func)
  // 	{
  // 		try	{  result = (*func)();	}
  // 		catch (...)	{  result = nuiDynamicLibraryAlreadyUnloaded;	}
  // 	}
  // 
  // 	registerPluginParamsMap.clear();
  // 	dynamicLibraryFreeVector.clear();
  // 	return result;
  return nuiPluginFrameworkErrorCode::Success;
}

bool nuiPluginManager::validate(const nuiRegisterModuleParameters *params)
{
  return 
   !((params == NULL) || 
    (params->allocateFunc == NULL) || 
    (params->deallocateFunc == NULL) || 
    (params->getDescriptorFunc == NULL));
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::registerModule(const nuiRegisterModuleParameters *params)
{
  nuiPluginManager pm = nuiPluginManager::getInstance();
  
  if(!pm.loadingPlugin)
    return nuiPluginFrameworkErrorCode::UnexpectedError;
  //if unexpected error occured
  
  if (!nuiPluginManager::validate(params))
    return nuiPluginFrameworkErrorCode::ModuleRegistrationFailed;

  nuiPluginFrameworkVersion version = pm.pluginFrameworkService.version;

  if (version.major != params->version.major)
    return nuiPluginFrameworkErrorCode::IncompatibleVersion;
  // perform checks whether we can register module

  for (int i=0; i<pm.modulesLoaded.size(); i++)
  {
    if(params->guid == pm.modulesLoaded[i]->guid)
    {
      return nuiPluginFrameworkErrorCode::RepeatingGUID;
    }
  }
  // check if already registered module

  nuiModuleLoaded* module = new nuiModuleLoaded(params);
  module->setParentPlugin(pm.loadingPlugin);
  // create wrapper for module management

  pm.loadingModules.push_back(module);
  // load module wrapper to list of currently loading modules

  return nuiPluginFrameworkErrorCode::Success;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadModule(GUID guid)
{
  for(int i=modulesLoaded.size()-1 ; i>=0 ; i--)
    if(modulesLoaded[i]->guid == guid)
    {
      modulesLoaded[i]->clearInstances();
      //! \todo remove module from parent plugin
      modulesLoaded.erase(modulesLoaded.begin() + i - 1);
    }
  return nuiPluginFrameworkErrorCode::Success;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadLibrary(const std::string path)
{
  if(loadingPlugin)
    return nuiPluginFrameworkErrorCode::UnexpectedError;
  
  //! \todo better error handling - consider using something better than string
  std::string errorString;
  nuiDynamicLibrary *library = nuiDynamicLibrary::load(path, errorString);
  if (!library)
    return nuiPluginFrameworkErrorCode::PluginLoadingFailed;
  else
  {
    loadingPlugin = new nuiPluginLoaded(library);
  }

  nuiLibraryLoadFunc initFunc = 
    (nuiLibraryLoadFunc)(library->getSymbol("nuiLibraryLoad"));

  if (initFunc == NULL)
    return nuiPluginFrameworkErrorCode::EntryPointNotFound;

  //send plugin framework service to plugin so modules could register themselves
  nuiPluginFrameworkErrorCode::err error = initFunc(&pluginFrameworkService);
  
  if(error == nuiPluginFrameworkErrorCode::Success)
  {
    // move loaded Module to modules
    modulesLoaded.insert(modulesLoaded.end(), loadingModules.begin(), 
      loadingModules.end());
    pluginsLoaded.push_back(loadingPlugin);
  }
  // check whether plugins were loaded correctly

  loadingModules.clear();
  loadingPlugin = NULL;
  
  return nuiPluginFrameworkErrorCode::Success;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadLibrary(const std::string path)
{
  //! \todo realization
//   if (dynamicLibraryMap.find(path) != dynamicLibraryMap.end())   
//   {
//     return unloadLibrary(dynamicLibraryMap[path]);
//   }
//   return nuiDynamicLibraryAlreadyUnloaded;
   return nuiPluginFrameworkErrorCode::Success;
}

// nuiPluginFrameworkErrorCode nuiPluginManager::queryPluginObject(const nuiPluginEntity **pluginObject,const std::string& objectType)
// {
//   *pluginObject = NULL;
//   nuiObjectParameters np;
//   np.objectType = (const char*)objectType.c_str();
//   np.frameworkServices = &pluginFrameworkService;
//   if (registerPluginParamsMap.find(objectType) != registerPluginParamsMap.end())
//   {
//     nuiRegisterModuleParameters* rp = &registerPluginParamsMap[objectType];
//     void * object = rp->allocateFunc(&np);
//     if (object == NULL)
//       return nuiPluginObjectQueryingFailed;
//     pluginInstanceMap[objectType]->push_back((void*)object);
// 
//     // confirm that plugin was registered
//     *pluginObject = new nuiPluginEntity(objectType.c_str(),object,rp->deallocateFunc);
//     if (*pluginObject == NULL)
//       return nuiPluginNotRegistered;
//     return nuiPluginFrameworkOK;
//   }
//   return nuiPluginNotRegistered;
// }