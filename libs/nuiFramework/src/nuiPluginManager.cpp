#include "nuiDynamicLibrary.h"
#include "nuiPluginManager.h"
#include <string>

static bool isValid(const char *objectType, const nuiRegisterPluginParameters *params)
{
  if (!objectType || !(*objectType))
     return false;
  return !((params == NULL) || 
	  (params->queryInterfaceFunc == NULL) || 
	  (params->releaseInterfaceFunc == NULL) || 
	  (params->queryModuleDescriptorFunc==NULL));
}

namespace nuiPluginFramework
{
	nuiPluginManager::nuiPluginManager()
	{
		pluginFrameworkService.version.major = MAJOR_VERSION;
		pluginFrameworkService.version.minor = MINOR_VERSION;
		pluginFrameworkService.registerPluginType = registerPluginType;
	}

	nuiPluginManager *nuiPluginManager::getInstance()
	{
		static nuiPluginManager *instance = NULL;
		if (instance == NULL)
			instance = new nuiPluginManager();
		return instance;
	}

	nuiPluginManager::~nuiPluginManager()
	{
		shutdown();
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::shutdown()
	{
		nuiPluginFrameworkErrorCode result = nuiPluginFrameworkOK;
		for (nuiDynamicLibraryMap::iterator i = dynamicLibraryMap.begin(); i != dynamicLibraryMap.end();i++)
			unloadLibrary(i->second);
		for (nuiDynamicLibraryFreeVec::iterator func = dynamicLibraryFreeVector.begin(); func != dynamicLibraryFreeVector.end(); ++func)
		{
			try	{  result = (*func)();	}
			catch (...)	{  result = nuiDynamicLibraryAlreadyUnloaded;	}
		}
		registerPluginParamsMap.clear();
		dynamicLibraryFreeVector.clear();
		return result;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::registerPluginType(const char *pluginType, const nuiRegisterPluginParameters *params)
	{
		if (!isValid(pluginType, params))
			return nuiPluginRegistrationFailed;
		nuiPluginManager* pm = nuiPluginManager::getInstance();
		nuiPluginFrameworkVersion version = pm->pluginFrameworkService.version;
		if (version.major != params->version.major)
			return nuiPluginNonCompatibleVersion;
		std::string key((const char *)pluginType);
		if (pm->registerPluginParamsMap.find(key) != pm->registerPluginParamsMap.end())    
			return nuiPluginAlreadyRegistered;
		pm->registerPluginParamsMap[key] = *params;
		pm->pluginInstanceMap[key] = new std::vector<void*>();
		if (pm->currentlyLoadingLibary!=NULL)
		{
			if (pm->dynamicLibraryPluginMap.find(pm->currentlyLoadingLibary)==pm->dynamicLibraryPluginMap.end())
				pm->dynamicLibraryPluginMap[pm->currentlyLoadingLibary] = new std::vector<std::string>();
			pm->dynamicLibraryPluginMap[pm->currentlyLoadingLibary]->push_back(key);
			if (pm->pluginInstanceMap.find(key)==pm->pluginInstanceMap.end())
				pm->pluginInstanceMap[key] = new std::vector<void*>();
		}
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::queryPluginObject(const nuiPluginEntity **pluginObject,const std::string& objectType)
	{
		*pluginObject = NULL;
		nuiObjectParameters np;
		np.objectType = (const char*)objectType.c_str();
		np.frameworkServices = &pluginFrameworkService;
		if (registerPluginParamsMap.find(objectType) != registerPluginParamsMap.end())
		{
			nuiRegisterPluginParameters* rp = &registerPluginParamsMap[objectType];
			void * object = rp->queryInterfaceFunc(&np);
			if (object == NULL)
				return nuiPluginObjectQueryingFailed;
			pluginInstanceMap[objectType]->push_back((void*)object);
			*pluginObject = new nuiPluginEntity(objectType.c_str(),object,rp->releaseInterfaceFunc);
			if (*pluginObject == NULL)
				return nuiPluginNotRegistered;
			return nuiPluginFrameworkOK;
		}
		return nuiPluginNotRegistered;
	}
	
	nuiRegisterPluginParamsMap *nuiPluginManager::getRigisteredPlugins()
	{
		return &registerPluginParamsMap;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::loadLibrary(const std::string &path)
	{
		std::string errorString;
		nuiDynamicLibrary *d = nuiDynamicLibrary::load(path, errorString);
		if (!d)
		  return nuiDynamicLibraryLoadingFailed;
		if (dynamicLibraryMap.find(path) != dynamicLibraryMap.end())
			return nuiDynamicLibraryAlreadyLoaded;
		dynamicLibraryMap[path] = d;
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::loadPluginsFromLibrary(nuiDynamicLibrary *dynamicLibrary)
	{
		if (dynamicLibrary == NULL)
			return nuiDynamicLibraryAlreadyUnloaded;
		nuiDynamicLibraryLoadFunc initFunc = (nuiDynamicLibraryLoadFunc)(dynamicLibrary->getSymbol("nuiDynamicLibraryLoad"));
		if (initFunc == NULL)
			return nuiDynamicLibraryEntryPointLoadingFailed;
		currentlyLoadingLibary = dynamicLibrary;
		nuiDynamicLibraryFreeFunc exitFunc = initFunc(&pluginFrameworkService);
		currentlyLoadingLibary = NULL;
		if (exitFunc == NULL)
			return nuiDynamicLibraryExitPointLoadingFailed;
		dynamicLibraryFreeVector.push_back(exitFunc);
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::loadPluginsFromLoadedLibraries()
	{
		for (nuiDynamicLibraryMap::iterator i = dynamicLibraryMap.begin();i != dynamicLibraryMap.end();i++)
		{
			loadPluginsFromLibrary(i->second);
		}
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::unloadPlugin(const std::string &pluginName)
	{
		if (pluginInstanceMap.find(pluginName) == pluginInstanceMap.end())   
			return nuiPluginIstancesAlreadyRemoved;
		std::vector<void*>* instances = pluginInstanceMap[pluginName];
		if (registerPluginParamsMap.find(pluginName) != registerPluginParamsMap.end())
		{
			nuiRegisterPluginParameters &rp = registerPluginParamsMap[pluginName];
			for ( std::vector<void*>::iterator i = instances->begin(); i != instances->end(); i++)
				rp.releaseInterfaceFunc(*i);
		}
		pluginInstanceMap.erase(pluginName);
		instances->clear();
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::unloadLibrary(nuiDynamicLibrary *dynamicLibrary)
	{
		std::string address = "";
		for ( nuiDynamicLibraryMap::iterator i = dynamicLibraryMap.begin(); i != dynamicLibraryMap.end(); i++)
		{
			if (i->second == dynamicLibrary)
			{
				address = i->first;
				break;
			}
		}
		if (address == "")
			return nuiDynamicLibraryAlreadyUnloaded;
		dynamicLibraryMap.erase(address);
		if (dynamicLibraryPluginMap.find(dynamicLibrary) == dynamicLibraryPluginMap.end())   
			return nuiPluginDescriptionAlreadyRemoved;
		std::vector<std::string>* pluginNames = dynamicLibraryPluginMap[dynamicLibrary];
		dynamicLibraryPluginMap.erase(dynamicLibrary);
		for ( std::vector<std::string>::iterator i = pluginNames->begin(); i != pluginNames->end(); i++)
			unloadPlugin(*i);
		pluginNames->clear();
		delete pluginNames;
		delete dynamicLibrary;
		return nuiPluginFrameworkOK;
	}

	nuiPluginFrameworkErrorCode nuiPluginManager::unloadLibrary(const std::string &path)
	{
		if (dynamicLibraryMap.find(path) != dynamicLibraryMap.end())   
		{
			return unloadLibrary(dynamicLibraryMap[path]);
		}
		return nuiDynamicLibraryAlreadyUnloaded;
	}
}








