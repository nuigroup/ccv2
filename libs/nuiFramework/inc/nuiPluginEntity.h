/* nuiPluginManager.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_PLUGIN_WRAPPER_
#define _NUI_PLUGIN_WRAPPER_

#include "nuiPlugin.h"

namespace nuiPluginFramework
{
	class nuiPluginManager;
	class nuiDynamicLibrary;

	class nuiPluginEntity
	{
	public:
		const void* getEntity() { return entity; }
		const char* getName() { return name; }
		const bool isInitialized() { return initialized; }
		~nuiPluginEntity();
	private:
		nuiPluginEntity(const char* entityName,void* wrappedEntity, nuiReleaseInterface releaseInterface);
		void* entity;
		bool initialized;
		const char* name;
		nuiReleaseInterface releaseInterfaceFunc;
		friend class nuiPluginManager;
	};
}

#endif//_NUI_PLUGIN_WRAPPER_