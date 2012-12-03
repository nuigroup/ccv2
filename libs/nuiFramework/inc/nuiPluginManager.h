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
#include <map>
#include <vector>
#include <string>

//! \def MAJOR_VERSION defines major version of service that loads plugins
#define MAJOR_VERSION 1
//! \def MINOR_VERSION defines minor version of service that loads plugins
#define MINOR_VERSION 0

class nuiFactory;
class nuiDynamicLibrary;

//! maps string library address to instances of nuiDynamicLibrary
typedef std::map<std::string, nuiDynamicLibrary*> nuiDynamicLibraryMap; 

//! maps instances of nuiDynamicLibrary to vectors_of_plugin_names, contained in that dll
typedef std::map<nuiDynamicLibrary*,std::vector<std::string>*> nuiDynamicLibraryPluginMap;

//! TODO : comment needs revision
//! maps plugin names to vectors_of_their_instances 
typedef std::map<std::string, std::vector<void*>*> nuiPluginInstanceMap;

//! vector of functions intended for dynamic library deallocation
typedef std::vector<nuiDynamicLibraryFreeFunc> nuiDynamicLibraryFreeVec;

//! TODO : comment needs revision
//! maps plugin_names to structures, containing pointers to core plugin functions
typedef std::map<std::string, nuiRegisterPluginParameters> nuiRegisterPluginParamsMap;

//! Singleton class 
class nuiPluginManager
{
public:
	//! gets instance of nuiPluginManager
	static nuiPluginManager *getInstance();

    /** \fn nuiPluginFrameworkErrorCode registerPluginType(const char *pluginType, const nuiRegisterPluginParameters *params)
     *  Called by dynamic library and used for sending data about plugin to nuiPluginManager. 
     *  Can be called several times by each dll if it contains more than one plugin.
     *  \return result of this operation to dynamic library
     */
    static nuiPluginFrameworkErrorCode registerPluginType(const char *pluginType, const nuiRegisterPluginParameters *params);

private:
	//! private constructor
	nuiPluginManager();
	//!private destructor
	~nuiPluginManager();

public:
	/*! \fn nuiPluginFrameworkErrorCode loadLibrary(const std::string &path)
     *  load dynamic library with specific path and add to internal collection
	 *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode loadLibrary(const std::string &path);

	/*! initialize plugin infos from specified dynamic library
	 *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode loadPluginsFromLibrary(nuiDynamicLibrary *dynamicLibrary);
		
	/*! initialize plugin infos from all loaded dynamic library
	 *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode loadPluginsFromLoadedLibraries();

	/** unload specified library and delete all previously allocated instances 
     *  and plugin infos stored in this library
	 *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode unloadLibrary(nuiDynamicLibrary *dynamicLibrary);

	/** unload specified library and delete all previously allocated instances 
     *  and plugin infos stored in this library
     *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode unloadLibrary(const std::string &path);

	/** unload specified plugin and delete all previously allocated instances of this plugin
	 *  \return result of this operation
     */
	nuiPluginFrameworkErrorCode unloadPlugin(const std::string &pluginName);

    //! \return map plugin_name - structure, containing core plugin management functions
	nuiRegisterPluginParamsMap *getRigisteredPlugins();

    //! creates plugin instance, pushes it to pluginInstanceMap
	nuiPluginFrameworkErrorCode queryPluginObject(const nuiPluginEntity **pluginObject,const std::string& pluginTypeName);

private:
	nuiDynamicLibrary* currentlyLoadingLibary;
	nuiPluginFrameworkService pluginFrameworkService;
	nuiRegisterPluginParamsMap registerPluginParamsMap;
	nuiDynamicLibraryFreeVec dynamicLibraryFreeVector;
	nuiDynamicLibraryMap dynamicLibraryMap;
	nuiDynamicLibraryPluginMap dynamicLibraryPluginMap;
	nuiPluginInstanceMap pluginInstanceMap;

	//! plugin manager shutdown logic
	nuiPluginFrameworkErrorCode shutdown();

	friend class nuiFactory;
};
#endif//NUI_PLUGIN_MANAGER_H