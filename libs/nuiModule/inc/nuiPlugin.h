/* nuiPlugin.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/
#ifndef NUI_PLUGIN_H
#define NUI_PLUGIN_H

enum nuiPluginFrameworkErrorCode
{
	nuiPluginFrameworkOK,
	nuiPluginRegistrationFailed,
	nuiPluginNotRegistered,
	nuiPluginObjectQueryingFailed,
	nuiPluginObjectCastingFailed,
	nuiPluginAlreadyRegistered,
	nuiPluginNonCompatibleVersion,
	nuiPluginCreationError,
	nuiPluginReleaseError,
	nuiPluginServiceNotFound,
	nuiPluginServiceWrongObjectParams,
	nuiPluginDescriptionAlreadyRemoved,
	nuiPluginIstancesAlreadyRemoved,
	nuiDynamicLibraryLoadingFailed,
	nuiDynamicLibraryAlreadyLoaded,
	nuiDynamicLibraryAlreadyUnloaded,
	nuiDynamicLibraryEntryPointLoadingFailed,
	nuiDynamicLibraryExitPointLoadingFailed
};

struct nuiPluginFrameworkService;

typedef struct nuiObjectParameters
{
  const char *objectType;
  const struct nuiPluginFrameworkService *frameworkServices;
} nuiObjectParameters;

typedef struct nuiPluginFrameworkVersion
{
  int major;
  int minor;
} nuiPluginFrameworkVersion;


typedef void *(*nuiQueryInterface)(nuiObjectParameters *); 
typedef nuiPluginFrameworkErrorCode (*nuiReleaseInterface)(void *);
typedef nuiPluginFrameworkErrorCode (*nuiPluginManagerLoadedCallback)();
class nuiModuleDescriptor;
typedef nuiModuleDescriptor *(*nuiQueryDescriptor)();

typedef struct nuiRegisterPluginParameters
{
  nuiPluginFrameworkVersion version;
  nuiQueryInterface queryInterfaceFunc;
  nuiReleaseInterface releaseInterfaceFunc;
  nuiQueryDescriptor queryModuleDescriptorFunc;
  nuiPluginManagerLoadedCallback pluginManagerLoadedCallback;
} nuiRegisterPluginParameters;

typedef nuiPluginFrameworkErrorCode (*nuiRegisterPluginFunc)(const char *nodeType, const nuiRegisterPluginParameters *params);
typedef nuiPluginFrameworkErrorCode (*nuiInvokeServiceFunc)(const char *serviceName, void *serviceParams);
typedef int (*nuiEnumerateServiceFunc)(const char **serviceNames);
typedef nuiPluginFrameworkErrorCode (*nuiDynamicLibraryFreeFunc)();

typedef struct nuiPluginFrameworkService
{
  nuiPluginFrameworkVersion version;
  nuiRegisterPluginFunc registerPluginType; 
  nuiInvokeServiceFunc invokeService;
  nuiEnumerateServiceFunc enumerateServices;
 
} nuiPluginFrameworkService;

typedef nuiDynamicLibraryFreeFunc (*nuiDynamicLibraryLoadFunc)(const nuiPluginFrameworkService *);

#ifdef WIN32
	#define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" DLLEXPORT nuiDynamicLibraryFreeFunc nuiDynamicLibraryLoad(const nuiPluginFrameworkService *params);

#define IMPLEMENT_ALLOCATOR(type)						\
void *allocate##type(nuiObjectParameters* params)		\
{														\
	return (void*) (new type());						\
}														\

#define IMPLEMENT_DEALLOCATOR(type)											\
nuiPluginFrameworkErrorCode deallocate##type(void *object)					\
{																			\
	type* module = (type*)(object);											\
	if (module == NULL)														\
		return nuiPluginReleaseError;										\
	delete module;															\
	return nuiPluginFrameworkOK;											\
}																			\

#define START_IMPLEMENT_DESCRIPTOR(type,author,description)						\
nuiModuleDescriptor* descriptor##type = NULL;									\
nuiModuleDescriptor* get##type##Descriptor()									\
{																				\
	if (descriptor##type==NULL)													\
	{																			\
		descriptor##type = new nuiModuleDescriptor();							\
		nuiModuleDescriptor* descriptor = descriptor##type;						\
		descriptor->setName( #type );											\
		descriptor->setAuthor(author);											\
		descriptor->setDescription(description);								\
		descriptor->getProperties()["use_thread"] =  new nuiProperty(false);	\
		descriptor->getProperties()["oscillator_mode"] = new nuiProperty(false);\
		descriptor->getProperties()["oscillator_wait"] = new nuiProperty(15);	\
		descriptor->getProperties()["synced_input"] = new nuiProperty(true);	\
			
#define END_IMPLEMENT_DESCRIPTOR(type)										\
	}																		\
	return descriptor##type;												\
}																			\

#define REGISTER_PLUGIN(type,description,majorValue,minorValue)							\
registerParams->version.major = majorValue;												\
registerParams->version.minor = minorValue;												\
registerParams->queryInterfaceFunc = allocate##type;									\
registerParams->releaseInterfaceFunc = deallocate##type;								\
registerParams->queryModuleDescriptorFunc = get##type##Descriptor;						\
if (params->registerPluginType(description, registerParams) != nuiPluginFrameworkOK)	\
	return NULL;																		\
registerParams = new nuiRegisterPluginParameters();										\

#define START_MODULE_REGISTRATION()																					\
extern "C" DLLEXPORT nuiDynamicLibraryFreeFunc nuiDynamicLibraryLoad(const nuiPluginFrameworkService *params)		\
{																													\
	nuiRegisterPluginParameters *registerParams = new nuiRegisterPluginParameters();								\

#define END_MODULE_REGISTRATION()																					\
	delete registerParams;																							\
	return (nuiDynamicLibraryFreeFunc)ExitFunc;																								\
}																													\

#define START_MODULE_EXIT()																							\
extern "C" DLLEXPORT nuiPluginFrameworkErrorCode ExitFunc()															\
{																													\

#define END_MODULE_EXIT()																							\
	return nuiPluginFrameworkOK;																					\
}																													\

#endif//NUI_PLUGIN_H

