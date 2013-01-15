/** 
 * \file      nuiPluginEntity.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
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
	nuiPluginEntity(const char* entityName, void* wrappedEntity, nuiDeallocateFunc releaseInterface);

	void* entity;
	bool initialized;
	const char* name;
	nuiDeallocateFunc deallocateFunc;

	friend class nuiPluginManager;
};

#endif//NUI_PLUGIN_ENTITY_H