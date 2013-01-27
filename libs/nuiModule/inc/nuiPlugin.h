#ifndef NUI_PLUGIN_H
#define NUI_PLUGIN_H

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <string>

class nuiModuleDescriptor;
struct nuiPluginFrameworkService;

//! namespaced enum
struct nuiPluginFrameworkErrorCode
{
  enum err
  {
    Success,
    UnexpectedError,
    ModuleRegistrationFailed,
    PluginLoadingFailed,
    EntryPointNotFound,
    IncompatibleVersion,
    RepeatingModule,
    DefaultSettingsCorrupted
  };
};

struct nuiObjectParameters;
struct nuiPluginFrameworkVersion;
struct nuiRegisterModuleParameters;
struct nuiPluginFrameworkService;

typedef nuiModuleDescriptor *(*nuiGetDescriptorFunc)();
typedef void *(*nuiAllocateFunc)(nuiObjectParameters *); 
typedef nuiPluginFrameworkErrorCode::err (*nuiDeallocateFunc)(void *);
typedef nuiPluginFrameworkErrorCode::err (*nuiRegisterModuleFunc)(const nuiRegisterModuleParameters *params);
typedef nuiPluginFrameworkErrorCode::err (*nuiLibraryFreeFunc)();
typedef nuiPluginFrameworkErrorCode::err (*nuiLibraryLoadFunc)(const nuiPluginFrameworkService *);

//! structure passed to create plugin objects
struct nuiObjectParameters
{
  const char *objectType;
  const struct nuiPluginFrameworkService *frameworkServices;
};

//! structure holding version information
struct nuiPluginFrameworkVersion
{
  //! major changes affect plugin compatibility. Older plugins must be rebuilt with newer version of nuiModule
  int major;
  //! minor changes should not affect plugin compatibility
  int minor;
};

//! structure holding all information required to give framework control over module
struct nuiRegisterModuleParameters
{
  nuiPluginFrameworkVersion version;
  std::string name;
  nuiAllocateFunc allocateFunc;
  nuiDeallocateFunc deallocateFunc;
  nuiGetDescriptorFunc getDescriptorFunc;
};

//! services exposed by framework to plugin
struct nuiPluginFrameworkService
{
  nuiPluginFrameworkVersion version;
  nuiRegisterModuleFunc registerModule; 
};

extern "C" DLLEXPORT 
nuiPluginFrameworkErrorCode::err nuiLibraryLoad(const nuiPluginFrameworkService *params);

#define IMPLEMENT_ALLOCATOR(type)						        \
static void* allocate##type##(nuiObjectParameters* params)	  \
{														                        \
  return (void*)(new type());						          \
}														                        \

#define IMPLEMENT_DEALLOCATOR(type)											                       \
static nuiPluginFrameworkErrorCode::err deallocate##type##(void *object)				             \
{																			                                         \
  type* module = (type*)(object);											                         \
  if (module == NULL)														                               \
    return nuiPluginFrameworkErrorCode::Success;										                           \
  delete module;															                                 \
  return nuiPluginFrameworkErrorCode::Success;											                           \
}																			                                         \

#define START_IMPLEMENT_DESCRIPTOR(type,author,description)					           \
nuiModuleDescriptor* descriptor##type = NULL;							                     \
nuiModuleDescriptor* get##type##Descriptor()							                     \
{																			                                         \
  if (descriptor##type == NULL)												                           \
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

//! \todo add plugin name registration - through descriptor or registerParams
/** \def START_EXPORT_MODULES()
*  Starts plugin registration
*/
#define START_EXPORT_MODULES()									                               \
extern "C" DLLEXPORT                                                           \
nuiPluginFrameworkErrorCode::err nuiLibraryLoad(const nuiPluginFrameworkService *params)\
{																													                     \
  nuiRegisterModuleParameters *registerParams = new nuiRegisterModuleParameters();\
  nuiPluginFrameworkErrorCode::err error =                                     \
    nuiPluginFrameworkErrorCode::Success;                                      \

//! \todo rewrite plugin to handle multiple modules and register each of them
/** \def REGISTER_MODULE()
*  Fills module registration parameters
*  I.e. structure with module controlling functions
*/
#define REGISTER_MODULE(type,description,majorValue,minorValue) \
  registerParams->version.major = majorValue; \
  registerParams->version.minor = minorValue;	\
  registerParams->allocateFunc = allocate##type##; \
  registerParams->deallocateFunc = deallocate##type##; \
  registerParams->getDescriptorFunc = get##type##Descriptor; \
  registerParams->name = #type; \
  error = params->registerModule(registerParams); \
  if (error != nuiPluginFrameworkErrorCode::Success) \
    return error; \

/** \def END_EXPORT_MODULES()
*  Finalizes module registration function
*/
#define END_EXPORT_MODULES()																					     \
  delete registerParams;																							         \
  return error;																	 \
}																													                     \

/** \todo we can add some functions like onLoading, onLoaded, onUnloading, 
 ** onUnloaded to get some control over plugin loading process.
 */

#endif//NUI_PLUGIN_H

