/* nuiDynamicLibrary.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef NUI_DYNAMIC_LIBRARY_H
#define NUI_DYNAMIC_LIBRARY_H

#include <string>
 
#if defined(OSX)
  static std::string dynamicLibraryExtension("dylib");
#elif defined(UNIX)
  static std::string dynamicLibraryExtension("so");
#elif defined(WIN32)
  static std::string dynamicLibraryExtension("dll");
#endif

class nuiDynamicLibrary
{
public:
	static nuiDynamicLibrary* load(const std::string path, std::string &errorString);
  static bool unload(void* handle);

	~nuiDynamicLibrary();
	void* getSymbol(const std::string &name);
  void* getHandle();
  std::string getPath();
private:
	nuiDynamicLibrary();
	nuiDynamicLibrary(void* handle, std::string path);

	void* handle_;
  std::string pathToLibrary;
};

#endif//NUI_DYNAMIC_LIBRARY_H
