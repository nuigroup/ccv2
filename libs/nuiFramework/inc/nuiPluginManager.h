/* nuiPluginManager.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_PLUGIN_MANAGER_
#define _NUI_PLUGIN_MANAGER_

#include "nuiPlugin.h"
#include "nuiPluginEntity.h"
#include <map>
#include <vector>
#include <string>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

class nuiFactory;

namespace nuiPluginFramework
{
	class nuiDynamicLibrary;

	typedef std::map<std::string, nuiDynamicLibrary*> nuiDynamicLibraryMap; 
	typedef std::map<nuiDynamicLibrary*,std::vector<std::string>*> nuiDynamicLibraryPluginMap;
	typedef std::map<std::string,std::vector<void*>*> nuiPluginInstanceMap;
	typedef std::vector<nuiDynamicLibraryFreeFunc> nuiDynamicLibraryFreeVec;
	typedef std::map<std::string, nuiRegisterPluginParameters> nuiRegisterPluginParamsMap;
	
	class nuiPluginManager
	{
	public:
		//gets instance of nuiPluginManager
		static nuiPluginManager *getInstance();
	public:
		//private constructor
		nuiPluginManager();
		//private destructor
		~nuiPluginManager();
		//load dynamic library with specific path and add to internal collection
		//returns result of this operation
		nuiPluginFrameworkErrorCode loadLibrary(const std::string &path);
		//initialize plugin infos from specified dynamic library
		//returns result of this operation
		nuiPluginFrameworkErrorCode loadPluginsFromLibrary(nuiDynamicLibrary *dynamicLibrary);
		//initialize plugin infos from all loaded dynamic library
		//returns result of this operation
		nuiPluginFrameworkErrorCode loadPluginsFromLoadedLibraries();
		//unload specified library and delete all previously allocated instances and plugin infos stored in this library
		//returns result of this operation
		nuiPluginFrameworkErrorCode unloadLibrary(nuiDynamicLibrary *dynamicLibrary);
		//unload specified library and delete all previously allocated instances and plugin infos stored in this library
		//returns result of this operation
		nuiPluginFrameworkErrorCode unloadLibrary(const std::string &path);
		//unload specified plugin and delete all previously allocated instances of this plugin
		//returns result of this operation
		nuiPluginFrameworkErrorCode unloadPlugin(const std::string &pluginName);
		nuiRegisterPluginParamsMap *getRigisteredPlugins();
		nuiPluginFrameworkErrorCode queryPluginObject(const nuiPluginEntity **pluginObject,const std::string& pluginTypeName);		
	public:
		//called by dynamic library and used for sending data about plugin to nuiPluginManager. Can be called few times by each dynamic library if this contains more than 
		//one plugin object type. 
		//returns result of this operation to dynamic library
		static nuiPluginFrameworkErrorCode registerPluginType(const char *pluginType, const nuiRegisterPluginParameters *params);
	private:
		nuiDynamicLibrary* currentlyLoadingLibary;
		nuiPluginFrameworkService pluginFrameworkService;
		nuiRegisterPluginParamsMap registerPluginParamsMap;
		nuiDynamicLibraryFreeVec dynamicLibraryFreeVector;
		nuiDynamicLibraryMap dynamicLibraryMap;
		nuiDynamicLibraryPluginMap dynamicLibraryPluginMap;
		nuiPluginInstanceMap pluginInstanceMap;
		//list of services
		//plugin manager shutdown logic
		nuiPluginFrameworkErrorCode shutdown();
		friend class nuiFactory;
	};
}
#endif//_NUI_PLUGIN_MANAGER_