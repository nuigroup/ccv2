#include "nuiDynamicLibrary.h"
#include "nuiPluginManager.h"
#include <string>

nuiPluginManager *nuiPluginManager::getInstance()
{
    static nuiPluginManager *instance = NULL;
    if (instance == NULL)
        instance = new nuiPluginManager();
    return instance;
}

bool nuiPluginManager::isValid(const char *objectType, const nuiRegisterPluginParameters *params)
{
    // TODO: wtf?
    if (!objectType || !(*objectType))
        return false;
    return !((params == NULL) || 
        (params->allocateFunc == NULL) || 
        (params->deallocateFunc == NULL) || 
        (params->queryModuleDescriptorFunc==NULL));
}

nuiPluginManager::nuiPluginManager()
{
	pluginFrameworkService.version.major = MAJOR_VERSION;
	pluginFrameworkService.version.minor = MINOR_VERSION;
	pluginFrameworkService.pluginRegisterFunc = pluginRegisterFunc;
}

nuiPluginManager::~nuiPluginManager()
{
	shutdown();
}

nuiPluginFrameworkErrorCode nuiPluginManager::shutdown()
{
	nuiPluginFrameworkErrorCode result = nuiPluginFrameworkOK;
    nuiDynamicLibraryMap::iterator i;
	for ( i = dynamicLibraryMap.begin(); i != dynamicLibraryMap.end(); i++)
		unloadLibrary(i->second);
    nuiDynamicLibraryFreeVec::iterator func;

    //! TODO : rewrite without try catch
	for (func = dynamicLibraryFreeVector.begin(); func != dynamicLibraryFreeVector.end(); ++func)
	{
		try	{  result = (*func)();	}
		catch (...)	{  result = nuiDynamicLibraryAlreadyUnloaded;	}
	}

	registerPluginParamsMap.clear();
	dynamicLibraryFreeVector.clear();
	return result;
}

nuiPluginFrameworkErrorCode nuiPluginManager::pluginRegisterFunc(const char *pluginType, const nuiRegisterPluginParameters *params)
{
  // perform checks whether we can register plugin
	if (!nuiPluginManager::isValid(pluginType, params))
		return nuiPluginRegistrationFailed;

	nuiPluginManager* pm = nuiPluginManager::getInstance();
	nuiPluginFrameworkVersion version = pm->pluginFrameworkService.version;

	if (version.major != params->version.major)
		return nuiPluginNonCompatibleVersion;

	std::string key((const char *)pluginType);
	if (pm->registerPluginParamsMap.find(key) != pm->registerPluginParamsMap.end())
		return nuiPluginAlreadyRegistered;

  // try to register plugin
	pm->registerPluginParamsMap[key] = *params;
	pm->pluginInstanceMap[key] = new std::vector<void*>();
  //! TODO: how can we get here, having currently currentlyLoadingLibary == NULL?
	if (pm->currentlyLoadingLibary != NULL)
	{
		if (pm->dynamicLibraryPluginMap.find(pm->currentlyLoadingLibary) == pm->dynamicLibraryPluginMap.end())
			pm->dynamicLibraryPluginMap[pm->currentlyLoadingLibary] = new std::vector<std::string>();
		pm->dynamicLibraryPluginMap[pm->currentlyLoadingLibary]->push_back(key);
		if (pm->pluginInstanceMap.find(key) == pm->pluginInstanceMap.end())
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
		void * object = rp->allocateFunc(&np);
		if (object == NULL)
			return nuiPluginObjectQueryingFailed;
		pluginInstanceMap[objectType]->push_back((void*)object);

        // confirm that plugin was registered
		*pluginObject = new nuiPluginEntity(objectType.c_str(),object,rp->deallocateFunc);
		if (*pluginObject == NULL)
			return nuiPluginNotRegistered;
		return nuiPluginFrameworkOK;
	}
	return nuiPluginNotRegistered;
}
	
nuiRegisterPluginParamsMap *nuiPluginManager::getRegisteredPlugins()
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

	nuiDynamicLibraryLoadFunc initFunc = 
        (nuiDynamicLibraryLoadFunc)(dynamicLibrary->getSymbol("nuiDynamicLibraryLoad"));
	
    if (initFunc == NULL)
		return nuiDynamicLibraryEntryPointLoadingFailed;
    
	currentlyLoadingLibary = dynamicLibrary;
    // send plugin framework service to plugin so it could register itself
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
			rp.deallocateFunc(*i);
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
