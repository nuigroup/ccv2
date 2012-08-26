/* nuiDynamicLibrary.h
*  
*  Created on 01/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_DYNAMIC_LIBRARY_
#define _NUI_DYNAMIC_LIBRARY_

#include <string>
 
#if defined(OSX)
  static std::string dynamicLibraryExtension("dylib");
#elif defined(UNIX)
  static std::string dynamicLibraryExtension("so");
#elif defined(WIN32)
  static std::string dynamicLibraryExtension("dll");
#endif

namespace nuiPluginFramework
{
	class nuiDynamicLibrary
	{
	public:
	  static nuiDynamicLibrary * load(const std::string & path, 
								   std::string &errorString);
	  ~nuiDynamicLibrary();
	  void * getSymbol(const std::string & name);
	private:
	  nuiDynamicLibrary();
	  nuiDynamicLibrary(void * handle);
	  nuiDynamicLibrary(const nuiDynamicLibrary &);
	private:
	  void * handle_;
	};
}

#endif//_NUI_DYNAMIC_LIBRARY_
