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

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// Forward - declarations
class nuiModuleDescriptor;
struct nuiPluginFrameworkService;

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

struct nuiObjectParameters
{
  const char *objectType;
  const struct nuiPluginFrameworkService *frameworkServices;
};

struct nuiPluginFrameworkVersion
{
  int major;
  int minor;
};

typedef void *(*nuiAllocateFunc)(nuiObjectParameters *); 
typedef nuiPluginFrameworkErrorCode (*nuiDeallocateFunc)(void *);
typedef nuiModuleDescriptor *(*nuiQueryDescriptor)();

struct nuiRegisterPluginParameters
{
  nuiPluginFrameworkVersion version;
  nuiAllocateFunc allocateFunc;
  nuiDeallocateFunc deallocateFunc;
  nuiQueryDescriptor queryModuleDescriptorFunc;
};

typedef nuiPluginFrameworkErrorCode (*nuiRegisterPluginFunc)(const char *nodeType, const nuiRegisterPluginParameters *params);
typedef nuiPluginFrameworkErrorCode (*nuiDynamicLibraryFreeFunc)();

typedef nuiDynamicLibraryFreeFunc (*nuiDynamicLibraryLoadFunc)(const nuiPluginFrameworkService *);

struct nuiPluginFrameworkService
{
  nuiPluginFrameworkVersion version;
  nuiRegisterPluginFunc pluginRegisterFunc; 
};

extern "C" DLLEXPORT 
nuiDynamicLibraryFreeFunc nuiDynamicLibraryLoad(const nuiPluginFrameworkService *params);

#define IMPLEMENT_ALLOCATOR(type)						        \
void *allocate##type(nuiObjectParameters* params)	  \
{														                        \
  return (void*) (new type());						          \
}														                        \

#define IMPLEMENT_DEALLOCATOR(type)											                       \
nuiPluginFrameworkErrorCode deallocate##type(void *object)				             \
{																			                                         \
  type* module = (type*)(object);											                         \
  if (module == NULL)														                               \
    return nuiPluginReleaseError;										                           \
  delete module;															                                 \
  return nuiPluginFrameworkOK;											                           \
}																			                                         \

#define START_IMPLEMENT_DESCRIPTOR(type,author,description)					           \
nuiModuleDescriptor* descriptor##type = NULL;							                     \
nuiModuleDescriptor* get##type##Descriptor()							                     \
{																			                                         \
  if (descriptor##type==NULL)												                           \
  {																			                                       \
    descriptor##type = new nuiModuleDescriptor();							                 \
    nuiModuleDescriptor* descriptor = descriptor##type;						             \
    descriptor->setName( #type );											                         \
    descriptor->setAuthor(author);											                       \
    descriptor->setDescription(description);								                   \
    descriptor->getProperties()["use_thread"] =  new nuiProperty(false);	     \
    descriptor->getProperties()["oscillator_mode"] = new nuiProperty(false);   \
    descriptor->getProperties()["oscillator_wait"] = new nuiProperty(15);	     \
    descriptor->getProperties()["synced_input"] = new nuiProperty(true);	     \

#define END_IMPLEMENT_DESCRIPTOR(type)										                     \
  }																		                                         \
  return descriptor##type;												                             \
}																			                                         \

/** \def START_MODULE_REGISTRATION()
*  Starts module registration i.e. filling structure, containing pointers
*  to core plugin management functions
*/
#define START_MODULE_REGISTRATION()											                       \
extern "C" DLLEXPORT                                                           \
nuiDynamicLibraryFreeFunc nuiDynamicLibraryLoad(const nuiPluginFrameworkService *params) \
{																													                     \
  nuiRegisterPluginParameters *registerParams = new nuiRegisterPluginParameters();\

/** \def REGISTER_PLUGIN()
*  Fills module registration parameters
*  I.e. structure with plugin controlling functions
*/
#define REGISTER_PLUGIN(type,description,majorValue,minorValue)				         \
  registerParams->version.major = majorValue;								                   \
  registerParams->version.minor = minorValue;								                   \
  registerParams->allocateFunc = allocate##type;				                       \
  registerParams->deallocateFunc = deallocate##type;				                   \
  registerParams->queryModuleDescriptorFunc = get##type##Descriptor;		       \
  if (params->pluginRegisterFunc(description, registerParams) != nuiPluginFrameworkOK)	\
    return NULL;														                                   \
  registerParams = new nuiRegisterPluginParameters();						               \

/** \def END_MODULE_REGISTRATION()
*  Finalizes module registration function
*/
#define END_MODULE_REGISTRATION()																					     \
  delete registerParams;																							         \
  return (nuiDynamicLibraryFreeFunc)ExitFunc;																	 \
}																													                     \

/** \def START_MODULE_EXIT()
*  Starts module exit function
*  One can add exit logic between this and END_MODULE_EXIT() macro
*/
#define START_MODULE_EXIT()																							       \
extern "C" DLLEXPORT                                                           \
nuiPluginFrameworkErrorCode ExitFunc()														             \
{																													                     \

/** \def END_MODULE_EXIT()
*  Finalizes module exit function
*/
#define END_MODULE_EXIT()																							         \
  return nuiPluginFrameworkOK;																					       \
}																													                     \

#endif//NUI_PLUGIN_H

