#ifdef WIN32
  #include <Windows.h>
#else
  #include <dlfcn.h>
#endif

#include "nuiDynamicLibrary.h"
#include <sstream>
#include <iostream>

namespace nuiPluginFramework
{
	nuiDynamicLibrary::nuiDynamicLibrary(void * handle) : handle_(handle)
	{
	}

	nuiDynamicLibrary::~nuiDynamicLibrary ()
	{
	  if (handle_)
	  {
	  #ifndef WIN32
		dlclose(handle_);
	  #else
		FreeLibrary((HMODULE)handle_);
	  #endif
		handle_ = NULL;
	  }
	}

	nuiDynamicLibrary *nuiDynamicLibrary::load(const std::string & name,  std::string & errorString)
	{
	  if (name.empty()) 
	  {
		errorString = "Empty path.";
		return NULL;
	  }
  
	  void * handle = NULL;

		#ifdef WIN32
		handle = LoadLibrary(name.c_str());
		if (handle == NULL)
		{
		  DWORD errorCode = GetLastError();
		  std::stringstream ss;
		  ss << std::string("LoadLibrary(") << name 
			 << std::string(") Failed. errorCode: ") 
			 << errorCode; 
		  errorString = ss.str();
		}
	  #else
		handle = dlopen(name.c_str(), RTLD_NOW);
		if (!handle) 
		{
		  std::string dlErrorString;
		  const char *zErrorString = ::dlerror();
		  if (zErrorString)
			dlErrorString = zErrorString;
		  errorString += "Failed to load \"" + name + '"';
		  if(dlErrorString.size())
			errorString += ": " + dlErrorString;
		  return NULL;
		}
	  #endif
	  return new nuiDynamicLibrary(handle);
	}

	void *nuiDynamicLibrary::getSymbol(const std::string &symbol)
	{
	  if (handle_ == NULL)
		return NULL;
	  #ifdef WIN32
		return GetProcAddress((HMODULE)handle_, symbol.c_str());
	  #else
		return dlsym(handle_, symbol.c_str());
	  #endif
	}
}