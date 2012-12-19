/* nuiPluginManager.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef NUI_PLUGIN_ENTITY_H
#define NUI_PLUGIN_ENTITY_H

#include "nuiPlugin.h"

//! \class nuiPluginEntity
//! Holds instantiated plugin
class nuiPluginEntity
{
public:
    //! simple getter
	const void* getEntity() { return entity; }

    //! simple getter
	const char* getName() { return name; }

    //! simple getter
	const bool isInitialized() { return initialized; }

	~nuiPluginEntity();
private:
    // Can only be called from nuiPluginManager
	nuiPluginEntity(const char* entityName, void* wrappedEntity, nuiReleaseInterface releaseInterface);

	void* entity;
	bool initialized;
	const char* name;
	nuiReleaseInterface releaseInterfaceFunc;

	friend class nuiPluginManager;
};

#endif//NUI_PLUGIN_ENTITY_H